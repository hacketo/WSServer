add_library(rapidjson STATIC
        allocators.h
        document.h
        encodedstream.h
        encodings.h
        filereadstream.h
        filewritestream.h
        fwd.h
        istreamwrapper.h
        memorybuffer.h
        memorystream.h
        ostreamwrapper.h
        pointer.h
        prettywriter.h
        rapidjson.h
        reader.h
        schema.h
        stream.h
        stringbuffer.h
        writer.h
        error/en.h
        error/error.h
        internal/biginteger.h
        internal/diyfp.h
        internal/dtoa.h
        internal/ieee754.h
        internal/itoa.h
        internal/meta.h
        internal/pow10.h
        internal/regex.h
        internal/stack.h
        internal/strfunc.h
        internal/strtod.h
        internal/swap.h
        msinttypes/inttypes.h
        msinttypes/stdint.h
)

set_target_properties(rapidjson PROPERTIES LINKER_LANGUAGE CXX)


