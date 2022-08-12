## 变数规则

1. 變數與變數內容以一個等號『=』來連結，如下所示：
   『myname=VBird』

2. 等號兩邊不能直接接空白字元，如下所示為錯誤：
   『myname = VBird』或『myname=VBird Tsai』

3. 變數名稱只能是英文字母與數字，但是開頭字元不能是數字，如下為錯誤：
   『2myname=VBird』

4. 變數內容若有空白字元可使用雙引號『"』或單引號『'』將變數內容結合起來，但

   - 雙引號內的特殊字元如 \$ 等，可以保有原本的特性，如下所示：
     『var="lang is \$LANG"』則『echo $var』可得『lang is zh_TW.UTF-8』
   - 單引號內的特殊字元則僅為一般字元 (純文字)，如下所示：
     『var='lang is \$LANG'』則『echo \$var』可得『lang is $LANG』

5. 可用跳脫字元『 \ 』將特殊符號(如 [Enter], $, \, 空白字元, '等)變成一般字元，如：
   『myname=VBird\ Tsai』

6. 在一串指令的執行中，**還需要藉由其他額外的指令所提供的資訊時**，可以使用反單引號**『\`指令\`』**或 **『\$(指令)』**。特別注意，那個 ` 是鍵盤上方的數字鍵 1 左邊那個按鍵，而不是單引號！ 例如想要取得核心版本的設定：
   『version=\$(uname -r)』再『echo $version』可得『3.10.0-229.el7.x86_64』

7. 若該變數為擴增變數內容時，則可用 "$變數名稱" 或 ${變數} 累加內容，如下所示：
   『PATH="\$PATH":/home/bin』或『PATH=${PATH}:/home/bin』

8. 若該變數需要在其他子程序執行，則需要以 export 來使變數變成環境變數：
   『export PATH』

9. 通常大寫字元為系統預設變數，自行設定變數可以使用小寫字元，方便判斷 (純粹依照使用者興趣與嗜好) ；

10. 取消變數的方法為使用 unset ：『unset 變數名稱』例如取消 myname 的設定：
    『unset myname』

| 變數設定方式     | str 沒有設定       | str 為空字串       | str 已設定非為空字串 |
| ---------------- | ------------------ | ------------------ | -------------------- |
| var=${str-expr}  | var=expr           | var=               | var=$str             |
| var=${str:-expr} | var=expr           | var=expr           | var=$str             |
| var=${str+expr}  | var=               | var=expr           | var=expr             |
| var=${str:+expr} | var=               | var=               | var=expr             |
| var=${str=expr}  | str=expr var=expr  | str 不變 var=      | str 不變 var=$str    |
| var=${str:=expr} | str=expr var=expr  | str=expr var=expr  | str 不變 var=$str    |
| var=${str?expr}  | expr 輸出至 stderr | var=               | var=$str             |
| var=${str:?expr} | expr 輸出至 stderr | expr 輸出至 stderr | var=$str             |