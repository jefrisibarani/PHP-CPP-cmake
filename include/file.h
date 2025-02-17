/**
 *  File.h
 *
 *  Extended script, a PHP source file name can be passed to a Php::File object
 *  to have it evaluated.
 *
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2014 Copernica BV
 */

/**
 *  Forward declarations
 */
struct _zend_string;

/**
 *  Set up namespace
 */
namespace Php {

/**
 *  Class definition
 */
class PHPCPP_EXPORT File
{
public:
    /**
     *  Constructor
     *
     *  The constructor receives a filename as parameter. It uses the normal
     *  PHP include path resolve algorithms to find the location of the file.
     *
     *  @param  name        the filename
     *  @param  size        size of the filename
     */
    File(const char *name, size_t size);

    /**
     *  Alternative constructor with just a filename
     *
     *  @param  name        the filename
     */
    File(const char *name);

    /**
     *  Alternative constructor with a string object
     *  @param  name        the filename
     */
    File(const std::string &name);

    /**
     *  Alternative constructor with a Value object
     *  @param  name        the filename
     */
    File(const Value &value);

    /**
     *  Destructor
     */
    virtual ~File();

    /**
     *  Does the file exist?
     *  @return boolean
     */
    bool exists();

    /**
     *  Is this a valid file?
     *  @return boolean
     */
    bool valid();

    /**
     *  Execute the file once (do nothing if the file already was executed)
     *  @return Php::Value
     */
    Value once();

    /**
     *  Execute the file
     *  @return Php::Value
     */
    Value execute();

private:
    /**
     *  The full resolved path name
     *  @var struct _zend_string*
     */
    struct _zend_string *_path = nullptr;

    /**
     *  The opcodes of this file
     *  @var std::unique_ptr<Opcodes>
     */
    std::unique_ptr<Opcodes> _opcodes;

    /**
     *  Compile the file
     *  @return bool
     */
    bool compile();

};

/**
 *  End of namespace
 */
}
