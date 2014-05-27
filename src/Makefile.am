bin_PROGRAMS = bitcoind

bitcoind_SOURCES = bignum.cpp bitcoind.cpp chainparams.cpp core.cpp init.cpp \
		   main.cpp noui.cpp script.cpp uint256.cpp util.cpp

# bitcoind_LDADD += $(BOOST_LIBS)
bitcoind_LDADD = -lboost_regex -lboost_system -lboost_filesystem -lboost_program_options -lboost_thread -lcrypto -ldb

