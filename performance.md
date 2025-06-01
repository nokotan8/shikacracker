# Performance

This application supports multithreading to improve performance, using a shared work buffer
where password candidates are inserted by producer thread(s) then removed and subsequently
hashed by consumer thread(s). Currently, this uses OpenSSL's hash functions which only
make use of the CPU.

## Benchmarks

Some rough measurements to demonstrate the performance gains from multithreading. Unless specified otherwise,
run time measurements are taken using GNU `time`.

### Hardware

**CPU:** AMD Ryzen 5 7600

The following commands are run on commit [cddf5a9](https://github.com/nokotan8/shikacracker/tree/cddf5a917cdbbf3aecc8101e95cd2bf4fbaa51bd),
where computing hashes in parallel is supported, but password candidates are generated in order by a single thread,
e.g. `0000, 0001, ..., 9998, 9999`. Thus, trying to reverse the hash of `9999` is essentially worst case performance
(for that particular mask).

| Description | Hashes | Command | Time (m:s) | CPU |
| ---------------------------------------------------------- | ------ | ------------------------------------------------------- | ---------- | ------ |
| Reverse MD5 hash of `99999999` with mask attack, 1 thread | $10^8$ | `./shikacracker -m 0 -a 3 -t 1 hashes ?d?d?d?d?d?d?d?d` | 7:28.5 | 8-9.5% |
| Reverse MD5 hash of `99999999` with mask attack, 2 threads | $10^8$ | `./shikacracker -m 0 -a 3 -t 2 hashes ?d?d?d?d?d?d?d?d` | 1:38.64 | 19-22% |
| Reverse MD5 hash of `99999999` with mask attack, 4 threads | $10^8$ | `./shikacracker -m 0 -a 3 -t 4 hashes ?d?d?d?d?d?d?d?d` | 0:55.6 | 38-40% |
