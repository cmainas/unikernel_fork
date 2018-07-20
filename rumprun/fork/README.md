Fork και execve τον server

Σε αυτό το φάκελο υπάρχουν οι κώδικες των εφαρμογών, του driver και διάφορα άλλα
απαραίτητα αρχεία για το χτίσιμο του rumprun. 

Για δοκιμή, αφού κάνουμε clone το repo του rumprun και τα submodules, 
κατεβάζουμε την έκδοση qemu-2.11.2 
(https://download.qemu.org/qemu-2.11.2.tar.xz) 
και αφού κάνουμε make στο qemu, αλλάζουμε στο kvm-all.c το QEMU_BIN βάζοντας το
path για το qemu-system-x86_64 και αλλάζουμε
στο script pre_build.sh το path του rumprun repo και τα αντίστοιχα του qemu
Στη συνέχεια εκτελούμε το script pre_build.sh που μεταφέρει το kvm-all.c στο 
qemu και το ξανακάνει make. Ύστερα, ελέγχει αρχικά αν η libunwind έχει γίνει 
patched και αν όχι τότε το κάνει αυτό χρησιμποιοώντας το as.patch. Έπειτα 
μεταφέρει τα αρχεία στους κατάλληλους φακέλους. Τέλος χτίζουμε το rumprun με τo
script build-rr.sh hw. 

Αφού τελειώσει το χτίσιμο, φτιάχνουμε τα bins του rumprun, για αυτό το σκοπό
μπορεί να χρησιμοποιηθεί και το Makefile που υπάρχει στον εκάστοτε φάκελο,
αλλάζοντας κατάλληλα στις 2 πρώτες γραμμές το path που έχει εγκατασταθεί το κάθε
βοηθητικό rumprun πρόγραμμα. 

Τέλος, ξεκινάμε τον unikernel χρησιμοποιώντας την εντολή
$ rumprun kvm -g "-vga none -nographic -device ivshmem-plain,memdev=hostmem -object memory-backend-file,size=1M,share,mem-path=/dev/shm/ivshmem,id=hostmem" -i client-rumprun.bin
```
Στο φάκελο test, υπάρχει ένα απλό παράδειγμα όπου ο server ανταλλάσει μηνύματα 
με τον client.
Αν όλα έχουν πάει καλά πρέπει να έχουν σταματήσει και οι 2 unikernels μετά από λίγο.
Στο /tmp/my_server.out βρίσκεται η έξοδος από του server unikernel.


