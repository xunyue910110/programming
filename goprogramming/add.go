package main
import "fmt"
func Add(x int,y int,channel chan int) {
    sum := library.Add(x,y)
      fmt.Println(sum)
        channel <- 1
      }
      func main() {
          n:=10
            channel := make(chan int ,n)
              for i:=0;i<n;i++{
                      go Add(1,i,channel)
                        }
                          for i:=0;i<n;i++{

                                  <- channel
                                    }
                                  }
