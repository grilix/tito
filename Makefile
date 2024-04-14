.default_target: test

test: build # TODO: fix?
	cmake --build tmp/build -t test
build: tmp
	cmake --build tmp/build
tmp:
	cmake -B tmp/build
