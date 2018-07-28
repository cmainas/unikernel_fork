Fork με migration

Σε αυτό το φάκελο υπάρχουν οι κώδικες των εφαρμογών, του driver και διάφορα άλλα
απαραίτητα αρχεία για το χτίσιμο του rumprun. 

Για δοκιμή, αφού κάνουμε clone το repo του rumprun και τα submodules, 
κατεβάζουμε την έκδοση qemu-2.11.2 
(https://download.qemu.org/qemu-2.11.2.tar.xz) 
και αφού κάνουμε make στο qemu, αλλάζουμε στο kvm-all.c το QEMU_BIN βάζοντας το
path για το qemu-system-x86_64 και αλλάζουμε
στο script pre_build.sh το path του rumprun repo και τα αντίστοιχα του qemu
Στη συνέχεια εκτελούμε το script pre_build.sh (με το command line option -h 
μπορείτε να δείτε περισσότερες πληροφορίες σχετικά με το script). 
Τέλος χτίζουμε το rumprun με τo script build-rr.sh hw. 

Στο φάκελο test, υπάρχει ένα απλό πρόγραμμα που κάνει fork και η μία διεργασία
διαβάζει από το pipe, ενώ η άλλη γράφει σε αυτό. 

Τέλος, ξεκινάμε τον unikernel χρησιμοποιώντας την εντολή
$ rumprun kvm -g "-vga none -nographic -device ivshmem-plain,memdev=hostmem -object memory-backend-file,size=1M,share,mem-path=/dev/shm/ivshmem,id=hostmem" -i test-rumprun.bin
```
Αν όλα έχουν πάει καλά πρέπει να έχουν σταματήσει και οι 2 unikernels μετά από λίγο.
Στο /tmp/my_server.out βρίσκεται η έξοδος από το vm παιδί.


