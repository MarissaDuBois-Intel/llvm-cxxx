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
      Check("trine state = other; Trine copy; if (state) {} otherwise {}",
            "trine other Trine otherwise");
      Check("trine other otherwise", "", marked: false);
      Check("trines another other_value otherwiseFlag _trine", "");
      Check("// trine other\\\r\notherwise\r\nTrine", "Trine");
      Check("/* trine\nother otherwise */ Trine", "Trine");
      Check("\"trine \\\" other\" 'x' u8\"otherwise\" trine", "trine");
      Check("R\"tag(trine\nother \" otherwise)tag\" other", "other");
      Check("u8R\"(otherwise)\" trine", "trine");
      Check("R\"tag(otherwise\ntrine", "");
      Check("/* otherwise\ntrine", "");
      Check("1'000; other; 0x1'ff; trine", "other trine");
      Check("\"otherwise\ntrine", "trine");
      Check("otherwise", "otherwise");
      Console.WriteLine("PASS: Visual Studio keyword scanner (12 cases)");
    }
  }
}
