package main

import (
     "fmt"
        "time"
      )

      func main() {
           for i := 0; i < 10; i++ {
                        go sum(i, i+10)
                           }

                              time.Sleep(time.Second * 5)
                            }

                            func sum(start, end int) int {
                                 var sum int = 0
                                    for i := start; i < end; i++ {
                                                 sum += i
                                                    }

                                                       fmt.Printf("Sum from %d to %d is %d\n", start, end, sum)
                                                          return sum
                                                        }
