TARGETS = \
	annotation-logger \
	annotation-logger-win64.exe \
	annotation-logger-win32.exe \
	serial-logger \
	serial-logger-win64.exe \
	serial-logger-win32.exe \
#	serial-logger-darwin-amd64 \
#	annotation-logger-darwin-amd64 \


all: ${TARGETS}

clean:
	touch ${TARGETS}
	rm    ${TARGETS}

mrproper: clean
	touch dummy~
	rm        *~


serial-logger: serial-logger.go
	go get github.com/tarm/serial
	go build serial-logger.go

serial-logger-win64.exe: serial-logger.go
	go get github.com/tarm/serial
	GOOS=windows GOARCH=amd64 go build -o serial-logger-win64.exe serial-logger.go

serial-logger-win32.exe: serial-logger.go
	go get github.com/tarm/serial
	GOOS=windows GOARCH=386 go build -o serial-logger-win32.exe serial-logger.go

serial-logger-darwin-amd64: serial-logger.go
	go get github.com/tarm/serial
	CGO_ENABLED=1 GOOS=darwin GOARCH=amd64 go build -o serial-logger-darwin-amd64 serial-logger.go

annotation-logger: annotation-logger.go
	go build annotation-logger.go

annotation-logger-win64.exe: annotation-logger.go
	GOOS=windows GOARCH=amd64 go build -o annotation-logger-win64.exe annotation-logger.go

annotation-logger-win32.exe: annotation-logger.go
	GOOS=windows GOARCH=386 go build -o annotation-logger-win32.exe annotation-logger.go

annotation-logger-darwin-amd64: annotation-logger.go
	GOOS=darwin GOARCH=amd64 go build -o annotation-logger-darwin-amd64 annotation-logger.go
