// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

const assert = require("node:assert/strict");
const fs = require("node:fs");
const path = require("node:path");

async function run(appRoot) {
  const bundled = name => require(path.join(appRoot, "node_modules.asar", name));
  const textmate = bundled("vscode-textmate");
  const oniguruma = bundled("vscode-oniguruma");
  await oniguruma.loadWASM(fs.readFileSync(path.join(
    appRoot, "node_modules.asar.unpacked", "vscode-oniguruma", "release", "onig.wasm")));
  const cppRoot = path.join(appRoot, "extensions", "cpp");
  const cppManifest = JSON.parse(fs.readFileSync(path.join(cppRoot, "package.json")));
  const paths = new Map(cppManifest.contributes.grammars.map(
    grammar => [grammar.scopeName, path.join(cppRoot, grammar.path)]));
  paths.set("source.cxxx", path.join(__dirname, "cxxx.tmLanguage.json"));
  const registry = new textmate.Registry({
    onigLib: Promise.resolve({
      createOnigScanner: patterns => new oniguruma.OnigScanner(patterns),
      createOnigString: text => new oniguruma.OnigString(text)
    }),
    loadGrammar: async scope => {
      const file = paths.get(scope);
      return file ? textmate.parseRawGrammar(fs.readFileSync(file, "utf8"), file) : null;
    }
  });
  const cxxx = await registry.loadGrammar("source.cxxx");
  const cpp = await registry.loadGrammar("source.cpp");
  function keywords(grammar, source) {
    let state = textmate.INITIAL;
    const result = [];
    for (const line of source.split(/\r?\n/)) {
      const parsed = grammar.tokenizeLine(line, state);
      for (const token of parsed.tokens) {
        const scope = token.scopes.find(s =>
          /^(storage\.type|constant\.language\.other|keyword\.control\.otherwise)\.cxxx$/.test(s));
        if (scope)
          result.push([line.slice(token.startIndex, token.endIndex), scope]);
      }
      state = parsed.ruleStack;
    }
    return result;
  }
  assert.deepEqual(keywords(cxxx, "trine state = other;\nTrine copy;\notherwise {}"), [
    ["trine", "storage.type.cxxx"], ["other", "constant.language.other.cxxx"],
    ["Trine", "storage.type.cxxx"], ["otherwise", "keyword.control.otherwise.cxxx"]
  ]);
  const ignored = [
    '// other trine otherwise\n/* Trine\nother */',
    '"trine \\" otherwise" \'x\' u8"other"',
    'R"tag(trine\nother " otherwise)tag"',
    "trines another other_value otherwiseFlag _trine",
    "1'000; 0x1'ff;"
  ];
  for (const source of ignored)
    assert.deepEqual(keywords(cxxx, source), [], source);
  assert.deepEqual(keywords(cpp, "trine state = other; otherwise {}"), []);
  const demo = fs.readFileSync(path.join(__dirname, "..", "..", "trine.cpp"), "utf8");
  const demoTokens = keywords(cxxx, demo);
  for (const word of ["trine", "Trine", "other", "otherwise"])
    assert(demoTokens.some(token => token[0] === word), `Demo missing ${word}`);
  registry.dispose();
  console.log(`PASS: VS Code grammar, including ${demoTokens.length} demo keyword tokens`);
}

module.exports = { run };
if (require.main === module)
  run(process.argv[2]).catch(error => { console.error(error); process.exitCode = 1; });
