/**
 *  File.cpp
 *
 *  Implementation file for the File class
 *
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2014 Copernica BV
 */

/**
 *  Dependencies
 */
#include "includes.h"

/**
 *  Namespace
 */
namespace Php {

/**
 *  Constructor
 *
 *  The constructor receives a filename as parameter. It uses the normal
 *  PHP include path resolve algorithms to find the location of the file.
 *
 *  @param  name        the filename
 *  @param  size        length of the filename
 */
File::File(const char *name, size_t size)
{
    // resolve the path
    _path = zend_resolve_path(name, size);
}

/*
Constructor implemented in .cpp file , not in .h because
MSVC Shared build error LNK2019: unresolved external symbol "const Php::File::`vftable'" (??_7File@Php@@6B@)
referenced in function "public: __cdecl Php::File::File(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &)" (??0File@Php@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z)
*/

/**
 *  Alternative constructor with just a filename
 *
 *  @param  name        the filename
 */
File::File(const char *name) : File(name, ::strlen(name)) {}

/**
 *  Alternative constructor with a string object
 *  @param  name        the filename
 */
File::File(const std::string &name) : File(name.c_str(), name.size()) {}

/**
 *  Alternative constructor with a Value object
 *  @param  name        the filename
 */
File::File(const Value &value) : File(value.stringValue()) {}

/**
 *  Destructor
 */
File::~File()
{
    // clean up path name
    if (_path) zend_string_release(_path);
}

/**
 *  Compile the file
 *  @return bool
 */
bool File::compile()
{
    // never works if the path is invalid
    if (!_path) return false;

    // is the file already compiled?
    if (_opcodes) return _opcodes->valid();

    // we are going to open the file
    zend_file_handle fileHandle;

    // open the file
    if (zend_stream_open(ZSTR_VAL(_path), &fileHandle) == FAILURE) return false;

    // make sure the path name is stored in the handle (@todo: is this necessary? do we need the copy?)
    if (!fileHandle.opened_path) fileHandle.opened_path = zend_string_copy(_path);

    // we need temporary compiler options
    CompilerOptions options(ZEND_COMPILE_DEFAULT);

    // create the opcodes
    _opcodes.reset(new Opcodes(zend_compile_file(&fileHandle, ZEND_INCLUDE)));

    // close the file handle
    zend_destroy_file_handle(&fileHandle);

    // done
    return _opcodes->valid();
}

/**
 *  Does the file exist?
 *  @return boolean
 */
bool File::exists()
{
    // it is of course not valid if the path could not be resolved
    if (!_path) return false;

    // if we have valid opcodes, we're sure that it exists
    if (_opcodes && _opcodes->valid()) return true;

    // retrieve stats
    struct stat buf;
    return stat(ZSTR_VAL(_path), &buf) == 0;
}

/**
 *  Is this a valid file?
 *  @return boolean
 */
bool File::valid()
{
    // check if file is compilable
    return compile();
}

/**
 *  Execute the file
 *  @return Value
 */
Value File::execute()
{
    // do we already have the opcodes?
    if (_opcodes) return _opcodes->execute();

    // try compiling the file
    if (!compile()) return nullptr;

    // add the entry to the list of included files
    zend_hash_add_empty_element(&EG(included_files), _path);

    // execute the opcodes
    return _opcodes->execute();
}

/**
 *  Execute a file only once
 *  @return Value
 */
Value File::once()
{
    // skip if the path is invalid
    if (!_path) return nullptr;

    // check if this file was already included
    if (zend_hash_exists(&EG(included_files), _path)) return nullptr;

    // execute the file
    return execute();
}

/**
 *  End of namespace
 */
}
