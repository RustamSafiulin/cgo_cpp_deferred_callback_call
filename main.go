package main

// #cgo CFLAGS: -I./cpp_proxy -g -Wall
// #cgo LDFLAGS: -L./cpp_proxy -lproxy -lstdc++
// #include <stdlib.h>
// #include <proxy.h>
//
// void notify_result_ready(int, char *);
import "C"

import (
	"fmt"
)

/*
	Need set LD_LIBRARY_PATH before run
*/
func main() {

	var initResult = C.initialize_service(C.notify_result_callback(C.notify_result_ready))
	fmt.Println("Init result")
	fmt.Println(initResult)

	var task = C.int(10)
	C.start_calculation(task)

	C.deinitialize_service()
}

//export notify_result_ready
func notify_result_ready(result C.int, description *C.char) {
	fmt.Println("ready")
	fmt.Println(result)
	fmt.Println(C.GoString(description))
}
