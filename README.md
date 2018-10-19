# lbx-tool
A small tool to unpack and edit lbx files.

## Building
You have to build [liblbx](https://github.com/MarcoKull/liblbx) before you can compile the tool. 

To build lbx-tool, using cmake command line, a typical workflow is:

1. Get the source code and change to it.
```bash
git clone https://github.com/MarcoKull/lbx-tool.git
cd lbx-tool
```

2. Create a build directory and change to it.
```bash
mkdir build
cd build
```

3. Run cmake to configure the build tree.
```bash
cmake -G "Unix Makefiles" ..
```

4. Compile the project.
```bash
cmake --build .
```

5. Install lbx-tool.
```bash
make install
```

## Commandline interface
```
lbx-tool <option> [parameter]

   option                    parameter            description
 --------------------------------------------------------------------------------------------------
  -a  | --add               <lbx> <file>         Add a <file> to <lbx>.
  -c  | --create            <path>               Create a new empty lbx file at <path>.
  -d  | --delete            <lbx> <nr>           Delete file at index <nr> in <lbx>.
  -f  | --files             <lbx>                Print number of files contained in <lbx>.
  -h  | --help                                   Show this help.
  -l  | --list              <lbx>                List contents of <lbx>.
  -p  | --print             <lbx>                Print a technical overview of <lbx>.
  -r  | --replace           <lbx> <nr> <file>    Replace file at index <nr> with <file> in <lbx>.
  -u0 | --unknown0          <lbx> <nr>           Set first unknown area of <lbx> to <nr>.
  -u1 | --unknown1          <lbx> <file>         Set second unknown area of <lbx> to <file>.
  -x  | --extract           <lbx> <nr> <path>    Extract file with index <nr> from <lbx> to <path>.
  -xu | --extract-unknown   <lbx> <path>         Extract second unknown area of <lbx> to <path>.
```