#
# prads - FIN signatures
# --------------------------
#
# .-------------------------------------------------------------------------.
# | The purpose of this file is to cover signatures for FIN packets         |
# | This database is looking for a caring maintainer.                       |
# `-------------------------------------------------------------------------'
#
# (C) Copyright 2009-2010 by Edward Fjellskål <edward@redpill-linpro.com>
#
# Submit all additions to the authors.
#
# IMPORTANT INFORMATION ABOUT THE INTERDEPENDENCY OF FIN
# ----------------------------------------------------------------
#
# Bla bla... :)
#
# IMPORTANT INFORMATION ABOUT DIFFERENCES IN COMPARISON TO SYN:
# ----------------------------------------------------------------
#
# Bla bla... :)

## Linux
#smallvalue(500>):54:1:52:N,N,T0:ATFN
#*:64:1:52:N,N,T:ATFN:Linux:2.6
46:64:1:52:N,N,T:ATFN:Linux:2.6
54:64:1:52:N,N,T:ATFN:Linux:2.6
108:64:1:52:N,N,T:ATFN:Linux:2.6
216:64:1:52:N,N,T:ATFN:Linux:2.6
14:64:1:52:N,N,T:ATFN:Linux:2.6 (newer)

S4:64:1:*:.:AFDN:Linux:2.6 arm

32736:64:1:40:.:AFN:Linux:2.0


## Freebsd
8326:64:1:52:N,N,T:ATFN!:Freebsd:freebsd.org

## Windows
*:128:1:*:.:AFDN:Windows:2008 Server (UC)
#64053:128:1:*(437):.:AFDN
#*:128:1:*:.:AFDN:Windows: 2008 Server (UC)
#62993:110:1:*(579):.:AFDN

# 87.238.43.133,[fin:17089:126:1:*(454):.:AFDN],[distance:2]
# 87.238.50.235,[fin:260:128:1:52:N,N,T0:ATFN],[uptime:379hrs],[distance:0]
# 87.238.45.226,[fin:65397:128:1:52:N,N,T0:ATFN],[uptime:57hrs],[distance:0]


