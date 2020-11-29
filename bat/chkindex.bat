
stree -q . | sort > \temp\pjsgot
type index | stree -@@ | sort > \temp\pjswant

diff \temp\pjswant \temp\pjsgot > \temp\chkindex.out
rem rm -f \temp\pjswant \temp\pjsgot
echo ****** result is in \temp\chkindex.out *******
