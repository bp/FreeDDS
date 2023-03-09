# FreeDDS

The Data Dictionary System (DDS) is an I/O system capable of handling multi-dimensional seismic datasets in various formats. It allows different processing systems to be used together.

A DDS dataset typically consists of Dictionary and Binary data. The Dictionary is a text file containing free-form Definitions that define the Binary data format and accumulated processing history. The Binary data is pointed to by the Dictionary and can be separate or attached to the end of the Dictionary. DDS also has multi-file support.

DDS is currently used in production on large Terabyte datasets with MPI and OpenMP parallelization and is being offered in open source by BP America Inc. under FreeDDS.
