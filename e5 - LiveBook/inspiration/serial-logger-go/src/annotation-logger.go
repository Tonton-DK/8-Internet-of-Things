package main

import (
    "fmt"
    "os"
    "time"
    "bufio"
)

func append2log (log *os.File, t0 time.Time, t1 time.Time, line string) {
    var fullline string = fmt.Sprintf("%d.%09d,%d.%09d,%s\n", t0.Unix(), t0.Nanosecond(), t1.Unix(), t1.Nanosecond(), line)
    
    if _, err := log.Write([]byte(fullline)) ; err != nil {
        fmt.Println(err)
        os.Exit(3)
    }
}

func main () {
    // guard: command line args
    if len(os.Args) != 2 {
        fmt.Printf("Syntax: %s FILENAME\n", os.Args[0])
        fmt.Printf("        %s alog.csv\n", os.Args[0])
        os.Exit(1)
    }
    var log_path   string = os.Args[1]
    
    // print out configuration
    fmt.Printf("annotation-logger: stdin -> %s\n", log_path)
    
    // open log file
    log, err := os.OpenFile(log_path, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
    if err != nil {
        fmt.Println(err)
        os.Exit(2)
    }
    
    // construct scanner
    scanner := bufio.NewScanner(os.Stdin)
    
    // service loop
    for {
        // first timestamp
        var t0 time.Time = time.Now()
        
        // read line
        scanner.Scan()
        
        // last timestamp
        var t1 time.Time = time.Now()
        
        // process line
        append2log(log, t0, t1, scanner.Text())
    }
}

