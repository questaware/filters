main ()

{ int i;
  char ch;
  printf("%c%c", 27, 'G');
  for (i = 128; i <= 255; ++i)
  { printf("%d is %c\n", i, i);
    read(0, &ch, 1);
  }
  printf("%c%c", 27, '2');
}
