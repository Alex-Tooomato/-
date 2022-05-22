## Qt编译框架

1. Qt引用了信号和槽机制，因此在语法上对C++进行了部分扩充，qt提供了moc软件对代码进行预处理。
2. Qt的ui文件需要用uic软件生成ui_filename.h文件供qt中的代码文件来引用。

## CMakelist配置

有了上面的两条补充知识，具体操作也很清楚了。我们只要在传统CMakelists.txt上加上moc和uic操作即可。具体为如下两条：

```CMAKE
QT4_WRAP_UI(UI widget.ui)
QT4_WRAP_CPP(UI_CPP  Widget.h)
```

处理后的文件名为UI和UI_CPP，在需要的地方引用即可。

