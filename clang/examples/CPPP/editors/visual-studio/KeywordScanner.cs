// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace CXXX {
  internal readonly struct KeywordSpan {
    internal readonly int Start;
    internal readonly int Length;
    internal KeywordSpan(int start, int length) {
      Start = start;
      Length = length;
    }
  }

  internal static class KeywordScanner {
    internal const string Marker = "// -*- mode: c+++ -*-";

    // Consume literals and comments before looking for complete keyword tokens.
    private static readonly Regex Tokens = new Regex(
        @"//(?:\\\r?\n|[^\r\n])*" +
        @"|/\*[\s\S]*?(?:\*/|\z)" +
        "|(?:u8|u|U|L)?R\"(?<delimiter>[^\\s()\\\\]{0,16})\\([\\s\\S]*?(?:\\)\\k<delimiter>\"|\\z)" +
        @"|\b[0-9][\w'.]*" +
        "|(?:u8|u|U|L)?\"(?:\\\\[\\s\\S]|[^\"\\\\\\r\\n])*(?:\"|(?=\\r?\\n|\\z))" +
        @"|(?:u8|u|U|L)?'(?:\\[\s\S]|[^'\\\r\n])*(?:'|(?=\r?\n|\z))" +
        @"|(?<keyword>\b(?:trine|other|otherwise|when)\b)",
        RegexOptions.Compiled);

    internal static List<KeywordSpan> Scan(string text) {
      var result = new List<KeywordSpan>();
      int newline = text.IndexOf('\n');
      string firstLine = newline < 0 ? text : text.Substring(0, newline);
      if (firstLine.Trim() != Marker)
        return result;
      foreach (Match match in Tokens.Matches(text)) {
        if (match.Groups["keyword"].Success)
          result.Add(new KeywordSpan(match.Index, match.Length));
      }
      return result;
    }
  }
}
