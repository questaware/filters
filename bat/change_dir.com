$ verify = f$verify(0)
$ if p1 .eqs. "" then exit
$
$ if p1 .eqs. ".."
$ then
$   dir = "[-]"
$ else
$ if f$locate(":",p1) .ne. f$length(p1)
$ then
$   dir = p1
$ else
$   dir = "["
$
$loop:
$   if f$extract(0,2,p1) .eqs. ".."
$   then
$      dir = dir + "-"
$      p1 = f$extract(3,99,p1)
$      goto loop
$   endif
$lp2:
$   if f$length(p1) .le. 0 then goto elp2
$   sl = f$locate("/",p1)
$   d = f$extract(0,sl,p1)
$   dir = dir + "." + d
$   p1 = f$extract(sl+1,99,p1)
$   goto lp2
$elp2:
$   dir = dir + "]"
$ endif
$ endif
$
$ write sys$output "Sd:" + dir
$
$ set def 'dir'
$ pwd
$ if verify .eq. 1
$ then
$   set verify
$ endif
