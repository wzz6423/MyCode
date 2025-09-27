.PHONE:all
all:
	@cd compileServer;\
	make;\
	cd -;\
	cd ojServer;\
	make;\
	cd -;

.PHONY:output
output:
	@mkdir -p output/compileServer;\
	mkdir -p output/ojServer;\
	cp -rf compileServer/compileServer output/compileServer;\
	cp -rf ojServer/ojServer output/ojServer;\
	cp -rf ojServer/config output/ojServer;\
	cp -rf ojServer/questions output/ojServer;\
	cp -rf ojServer/template output/ojServer;\
	cp -rf ojServer/wwwroot output/ojServer;

.PHONY:clean
clean:
	@cd compileServer;\
	make clean;\
	cd -;\
	cd ojServer;\
	make clean;\
	cd -;\
	rm -rf output/compileServer/compileServer;\
	rm -rf output/ojServer/ojServer;

.PHONY:cleanAll
cleanAll:
	@rm -rf output;