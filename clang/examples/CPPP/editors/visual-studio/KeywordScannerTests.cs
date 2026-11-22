// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

using System;
using System.Linq;

namespace CXXX {
  internal static class KeywordScannerTests {
    private static void Check(string source, string expected, bool marked = true) {
      string text = (marked ? KeywordScanner.Marker + "\r\n" : "") + source;
      string actual = string.Join(" ", KeywordScanner.Scan(text)
          .Select(span => text.Substring(span.Start, span.Length)));
      if (actual != expected)
        throw new Exception("Expected [" + expected + "], got [" + actual + "]");
    }

    private static void Main() {
      Check("trine state = other; trine copy; if (state) {} otherwise {}",
            "trine other trine otherwise");
      Check("trine other otherwise when", "", marked: false);
      Check("Trine trines another other_value otherwiseFlag _trine", "");
      Check("// trine other\\\r\notherwise\r\ntrine", "trine");
      Check("/* trine\nother otherwise */ trine", "trine");
      Check("\"trine \\\" other\" 'x' u8\"otherwise\" trine", "trine");
      Check("R\"tag(trine\nother \" otherwise)tag\" other", "other");
      Check("u8R\"(otherwise)\" trine", "trine");
      Check("R\"tag(otherwise\ntrine", "");
      Check("/* otherwise\ntrine", "");
      Check("1'000; other; 0x1'ff; trine", "other trine");
      Check("\"otherwise\ntrine", "trine");
      Check("otherwise", "otherwise");
      Check("when (state) { other: {}, true: {} }", "when other");
      Check("// when\n\"when\" R\"(when)\" whenever when_value", "");
      Check("if (s) {} else when (s) { other: {} } " +
            "otherwise if (s) {} else when (s) { true: {} } " +
            "otherwise when (s) { other: {} }",
            "when other otherwise when otherwise when other");
      Console.WriteLine("PASS: Visual Studio keyword scanner (16 cases)");
    }
  }
}
