CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(crawler)

ADD_EXECUTABLE(
  crawler crawler.c handleURLs.c bloomfilter.c queue.c DFA.c ternaryTree.c common.h bloomfilter.h
)
