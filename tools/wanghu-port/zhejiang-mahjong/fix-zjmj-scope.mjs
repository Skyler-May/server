// 把 MSVC 允许的 for 作用域泄漏（i 在循环里声明但函数后面继续用）修好：
// 对每个报错行，往上找最近的函数定义，在其 '{' 之后插入 "int i = 0;"
import fs from "node:fs";

const path = "D:/my-projects/game-project/.dsh-acl-report/zjmj/GameLogic.cpp";
let lines = fs.readFileSync(path, "utf8").split(/\r?\n/);

const errLines = [747, 1038, 1081, 1134]; // 1-based
const inserts = new Set();

const fnRe = /^(?:bool|void|BYTE|LONG|int|DWORD)\s+CGameLogic::\w+\s*\(/;

for (const err of errLines) {
    let fnIdx = -1;
    for (let i = err - 1; i >= 0; i--) {
        if (fnRe.test(lines[i])) { fnIdx = i; break; }
    }
    if (fnIdx < 0) { console.log(`第 ${err} 行：找不到所属函数`); continue; }
    // 找到函数体开始的 '{'
    let brace = -1;
    for (let i = fnIdx; i < lines.length; i++) {
        if (lines[i].includes("{")) { brace = i; break; }
    }
    if (brace < 0) { console.log(`第 ${err} 行：找不到函数体`); continue; }
    inserts.add(brace);
    console.log(`第 ${err} 行 -> 函数在第 ${fnIdx + 1} 行，插入到第 ${brace + 2} 行`);
}

const sorted = [...inserts].sort((a, b) => b - a);
for (const b of sorted) {
    lines.splice(b + 1, 0, "\tint i = 0;");
}

fs.writeFileSync(path, lines.join("\n"), "utf8");
console.log("完成，插入 " + sorted.length + " 处");
