# SE302-编译原理

作为对《现代编译原理》(Andrew W.Appel *et al.*)一书中的课后大作业的实现，本文件夹包含了从零开始实现一个tiger编程语言在X86-64机器上编译所需要的编译器的全部内容。本任务是分阶段进行的，具体安排为：
* **lab1 Straight-line Program Interpreter**: 针对tiger language的语法，实现一个简单的straight-line语言分析和解释器；
* **lab2 Lexical Analysis**: 使用lexical分析器解析tiger language；
* **lab3 Parsing**: 使用Yacc建立一个tiger语言的语法分析器，包括对于错误信息的处理；
* **lab4 Type Checking**: 基于文法规则和type tree，对已有编译器输出内容进行类型检查；
* **lab5 Frames & Translation to Trees**: 实现对于tiger language的语义树翻译，并且对内容的堆栈进行定义；
* **lab6 Instruction Selection, Liveness Analysis, Register Allocation**: 结合前面5个lab的内容，最终实现一个可以把tiger语言编写的源程序编译成X86-64机器可以直接运行的汇编代码。
