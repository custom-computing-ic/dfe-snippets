# About

This snippets tests if data stays persistently in BRAM between 2 consecutive runs of a kernel.

Use case:

```
double bla = 0.0;
for (int i = 0; i < n; i++)
{
   bla += KernelDo(...);
}
```

It's good to keep bla on-chip, but can we, and how portable it is?