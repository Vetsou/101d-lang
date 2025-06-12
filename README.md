# 101$ Programming Language
101$ is an inflation-based programming language currently in development.

# Syntax concept

```
# Single-line comment

FN add(a, b) [
  RET a + b;
]

#>
  Multi-line
  comment block
  #> Comment blocks can be nested <#
<#

VAR my_var = NIL;
VAR bool_var = TRUE;

VAR sum = add(4, 12);

IF (sum <> 13 || sum == 13) [
  PRINT sum;
] ELSE [
  PRINT "Other";
]

PRINT "Hello World";

VAR i = 0;
WHILE (i < 10) [
  PRINT `Value is {{ i }}`;
  i = i + 1;
]
```