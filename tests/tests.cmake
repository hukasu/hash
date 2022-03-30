add_test(
    NAME UnknownAlgorithm
    COMMAND hash MD4 -t ""
)

# Algorithm names tests
add_test(
    NAME KnowsMD5
    COMMAND hash MD5 -t ""
)
add_test(
    NAME KnowsMD5Alias
    COMMAND hash md5 -t ""
)

# MD5 tests
add_test(
    NAME MD5Test0
    COMMAND hash MD5 -t "" "d41d8cd98f00b204e9800998ecf8427e"
)
add_test(
    NAME MD5Test1
    COMMAND hash MD5 -t "a" "0cc175b9c0f1b6a831c399e269772661"
)
add_test(
    NAME MD5Test2
    COMMAND hash MD5 -t "abc" "900150983cd24fb0d6963f7d28e17f72"
)
add_test(
    NAME MD5Test3
    COMMAND hash MD5 -t "message digest" "f96b697d7cb7938d525a2f31aaf161d0"
)
add_test(
    NAME MD5Test4
    COMMAND hash MD5 -t "abcdefghijklmnopqrstuvwxyz" "c3fcd3d76192e4007dfb496cca67e13b"
)
add_test(
    NAME MD5Test5
    COMMAND hash MD5 -t "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" "d174ab98d277d9f5a5611c2c9f419d9f"
)
add_test(
    NAME MD5Test6
    COMMAND hash MD5 -t "12345678901234567890123456789012345678901234567890123456789012345678901234567890" "57edf4a22be3c955ac49da2e2107b67a"
)
add_test(
    NAME MD5Test7
    COMMAND hash MD5 -t "The quick brown fox jumps over the lazy dog" "9e107d9d372bb6826bd81d3542a419d6"
)
add_test(
    NAME MD5Test8
    COMMAND hash MD5 -t "The quick brown fox jumps over the lazy dog." "e4d909c290d0fb1ca068ffaddf22cbd0"
)

# WILL_FAIL property
set_tests_properties(
    UnknownAlgorithm

    PROPERTIES
    WILL_FAIL ON
)