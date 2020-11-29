BEGIN { p = ""; ct = 0;
  printf("test ! -d td && mkdir td; cd td; rm -f *\n");
}
{ if (p == "" && $0 != "") 
  { if (ct != 0)
      printf("EOF\n");
    printf("cat <<EOF > %s\n", $1);
    ct = ct+1
  };
  if ($0 != "")
    printf("%s\n", $0);
  p = $0
}
END { if (ct != 0)
    printf("EOF\n");
}
