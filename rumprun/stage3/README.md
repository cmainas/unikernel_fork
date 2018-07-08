Pipe μέσω shared memory (rumprun part 3)

Σε αυτό το φάκελο υπάρχουν οι κώδικες των εφαρμογών, του driver και διάφορα άλλα
απαραίτητα αρχεία για το χτίσιμο του rumprun. 

Για δοκιμή, αφού κάνουμε clone το repo του rumprun και τα submodules, αλλάζουμε
στο script pre_build.sh το path του rumprun repo και εκτελούμε
το script pre_build.sh που ελέγχει αρχικά αν η libunwind έχει γίνει patched και
αν όχι τότε το κάνει αυτό χρησιμποιοώντας το as.patch. Έπειτα μεταφέρει τα
αρχεία στους κατάλληλους φακέλους. Τέλος χτίζουμε το rumprun με τo script
build-rr.sh hw. 

Αφού τελειώσει το χτίσιμο, φτιάχνουμε τα bins του rumprun, για αυτό το σκοπό
μπορεί να χρησιμοποιηθεί και το Makefile που υπάρχει στον παρών φάκελο,
αλλάζοντας κατάλληλα στις 2 πρώτες γραμμές το path που έχει εγκατασταθεί το κάθε
βοηθητικό rumprun πρόγραμμα. 

Τέλος, ξεκινάμε τους unikernels χρησιμοποιώντας τις εντολές
$ rumprun kvm -g "-vga none -nographic -device ivshmem-plain,memdev=hostmem -object memory-backend-file,size=1M,share,mem-path=/dev/shm/ivshmem,id=hostmem" -i client-rumprun.bin
$ rumprun kvm -g "-vga none -nographic -device ivshmem-plain,memdev=hostmem -object memory-backend-file,size=1M,share,mem-path=/dev/shm/ivshmem,id=hostmem" -i server-rumprun.bin
```
Είναι σημαντικό να τρέξει πρώτα ο client και μετά ο server.
Αν όλα έχουν πάει καλά πρέπει να έχουν σταματήσει και οι 2 unikernels μετά από λίγο.


