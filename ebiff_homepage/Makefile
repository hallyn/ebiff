all: homepage/manual.html

homepage/manual.html: ../ebiff/doc/manual.html
	cp ../ebiff/doc/manual.html homepage/

../ebiff/doc/manual.html: ../ebiff/doc/doc.xslt
	make -C ../ebiff doc
