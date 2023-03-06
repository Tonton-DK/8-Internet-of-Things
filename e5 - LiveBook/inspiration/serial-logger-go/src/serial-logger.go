package main

import (
    "fmt"
    "os"
    "time"
    "github.com/tarm/serial"
    "strconv"
)

const BUFSIZE = 1024

var buf  []byte = make([]byte, BUFSIZE)
var bufi   int  = 0

var parity_map = map[byte] serial.Parity {
    'n': serial.ParityNone,
    'o': serial.ParityOdd,
    'e': serial.ParityEven,
    'm': serial.ParityMark,
    's': serial.ParitySpace,
}

var stopbits_map = map[int] serial.StopBits {
    1:  serial.Stop1,
    15: serial.Stop1Half,
    2:  serial.Stop2,
}

func append2log (log *os.File, t0 time.Time, t1 time.Time, line string) {
    var fullline string = fmt.Sprintf("%d.%09d,%d.%09d,%s\n", t0.Unix(), t0.Nanosecond(), t1.Unix(), t1.Nanosecond(), line)
    
    if _, err := log.Write([]byte(fullline)) ; err != nil {
        fmt.Println(err)
        os.Exit(3)
    }
}

func main () {
    // guard: command line args
    if len(os.Args) != 6 {
        fmt.Printf("Syntax: %s DEVICE BAUDRATE PARITY STOPBITS FILENAME\n", os.Args[0])
        fmt.Printf("        %s /dev/ttyACM0 9600 n 1 log.csv\n", os.Args[0])
        fmt.Printf("        %s COM1 9600 n 1 log.csv\n", os.Args[0])
        os.Exit(1)
    }
    var dev_path   string = os.Args[1]
    var dev_baud_s string = os.Args[2]
    var dev_par_s  string = os.Args[3]
    var dev_stop_s string = os.Args[4]
    var log_path   string = os.Args[5]
    
    // guard: stop sanity
    dev_baud_i, err := strconv.Atoi(dev_baud_s)
    if err != nil {
        fmt.Println("Error converting baudrate number")
        fmt.Println(err)
        os.Exit(1)
    }
    
    // guard: parity sanity
    if len(dev_par_s)!=1 {
        fmt.Println("Wrong parity length. Try one of [n,o,e,m,s] ...")
        os.Exit(1)
    }
    var dev_par_c byte = dev_par_s[0]
    if _, exists := parity_map[dev_par_c]; !exists {
        fmt.Println("Unknown parity. Try one of [n,o,e,m,s] ...")
        os.Exit(1)
    }
    
    // guard: stop sanity
    dev_stop_i, err := strconv.Atoi(dev_stop_s)
    if err != nil {
        fmt.Println("Error converting number of stopbits")
        fmt.Println(err)
        os.Exit(1)
    }
    if _, exists := stopbits_map[dev_stop_i]; !exists {
        fmt.Println("Unknown number of stop bits. Try one of [1,15,2] ...")
        os.Exit(1)
    }
    
    // print out configuration
    fmt.Printf("serial-logger: %s[%d,%s,%d] -> %s\n", dev_path, dev_baud_i, dev_par_s, dev_stop_i, log_path)
    
    // parse serial options
    var dev_par serial.Parity   = parity_map[dev_par_c]
    var dev_stop serial.StopBits = stopbits_map[dev_stop_i]
    
    // open log file
    log, err := os.OpenFile(log_path, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
    if err != nil {
        fmt.Println(err)
        os.Exit(2)
    }
    
    // open serial port
    c := &serial.Config{Name: dev_path, Baud: dev_baud_i, Parity: dev_par, StopBits: dev_stop}
    s, err := serial.OpenPort(c)
    if err != nil {
        fmt.Println(err)
        os.Exit(4)
    }
    
    // service loop
    for {
        // read line
        var t0 time.Time = time.Now()
        for {
            // blocking read
            n, err := s.Read(buf[bufi:bufi+1])
            if err != nil {
                fmt.Println(err)
                os.Exit(4)
            }
            
            // guard: disconnect case?
            if n==0 {
                fmt.Println("n==0")
                os.Exit(5)
            }
            
            bufi++
            
            // check exit condition
            if buf[bufi-1]=='\n' {
                break
            }
        }
        var t1 time.Time = time.Now()
        
        // process line
        append2log(log, t0, t1, string(buf[0:bufi-1]))
        
        // cleanup
        bufi = 0
    }
}

