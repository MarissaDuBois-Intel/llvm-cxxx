// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Utilities;

namespace CXXX {
  [Export(typeof(IClassifierProvider))]
  [ContentType("C/C++")]
  internal sealed class ClassifierProvider : IClassifierProvider {
    [Import]
    internal IClassificationTypeRegistryService Registry = null;

    public IClassifier GetClassifier(ITextBuffer buffer) {
      return buffer.Properties.GetOrCreateSingletonProperty(
          () => new Classifier(buffer, Registry.GetClassificationType("CXXX keyword")));
    }
  }

  internal static class Classifications {
    [Export(typeof(ClassificationTypeDefinition))]
    [Name("CXXX keyword")]
    [BaseDefinition("keyword")]
    internal static ClassificationTypeDefinition Keyword = null;
  }

  [Export(typeof(EditorFormatDefinition))]
  [ClassificationType(ClassificationTypeNames = "CXXX keyword")]
  [Name("CXXX keyword")]
  [UserVisible(false)]
  [Order(After = Priority.High)]
  internal sealed class KeywordFormat : ClassificationFormatDefinition {
    public KeywordFormat() {
      DisplayName = "C+++ keyword";
    }
  }

  internal sealed class Classifier : IClassifier {
    private readonly IClassificationType keyword;
    private readonly object gate = new object();
    private ITextSnapshot cachedSnapshot;
    private List<KeywordSpan> cachedKeywords;

    internal Classifier(ITextBuffer buffer, IClassificationType keyword) {
      this.keyword = keyword;
      buffer.Changed += OnChanged;
    }

    public event EventHandler<ClassificationChangedEventArgs> ClassificationChanged;

    private void OnChanged(object sender, TextContentChangedEventArgs args) {
      ClassificationChanged?.Invoke(this, new ClassificationChangedEventArgs(
          new SnapshotSpan(args.After, 0, args.After.Length)));
    }

    public IList<ClassificationSpan> GetClassificationSpans(SnapshotSpan span) {
      lock (gate) {
        if (cachedSnapshot != span.Snapshot) {
          cachedSnapshot = span.Snapshot;
          cachedKeywords = KeywordScanner.Scan(span.Snapshot.GetText());
        }
        var result = new List<ClassificationSpan>();
        foreach (var token in cachedKeywords) {
          if (token.Start >= span.End.Position)
            break;
          if (token.Start + token.Length <= span.Start.Position)
            continue;
          result.Add(new ClassificationSpan(
              new SnapshotSpan(span.Snapshot, token.Start, token.Length), keyword));
        }
        return result;
      }
    }
  }
}
