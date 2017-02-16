/*
 * schemastypes.c : implementation of the XML Schema Datatypes
 *             definition and validity checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */

#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_SCHEMAS_ENABLED

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/hash.h>
#include <libxml/valid.h>
#include <libxml/xpath.h>
#include <libxml/uri.h>

#include <libxml/xmlschemas.h>
#include <libxml/schemasInternals.h>
#include <libxml/xmlschemastypes.h>

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#define DEBUG

#ifndef LIBXML_XPATH_ENABLED
extern double xmlXPathNAN;
extern double xmlXPathPINF;
extern double xmlXPathNINF;
#endif

#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#define XML_SCHEMAS_NAMESPACE_NAME \
    (const xmlChar *)"http://www.w3.org/2001/XMLSchema"


static unsigned long powten[10] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000L,
    100000000L, 1000000000L
};


/* Date value */
typedef struct _xmlSchemaValDate xmlSchemaValDate;
typedef xmlSchemaValDate *xmlSchemaValDatePtr;
struct _xmlSchemaValDate {
    long		year;
    unsigned int	mon	:4;	/* 1 <=  mon    <= 12   */
    unsigned int	day	:5;	/* 1 <=  day    <= 31   */
    unsigned int	hour	:5;	/* 0 <=  hour   <= 23   */
    unsigned int	min	:6;	/* 0 <=  min    <= 59	*/
    double		sec;
    unsigned int	tz_flag	:1;	/* is tzo explicitely set? */
    int			tzo	:11;	/* -1440 <= tzo <= 1440 */
};

/* Duration value */
typedef struct _xmlSchemaValDuration xmlSchemaValDuration;
typedef xmlSchemaValDuration *xmlSchemaValDurationPtr;
struct _xmlSchemaValDuration {
    long	        mon;		/* mon stores years also */
    long        	day;
    double		sec;            /* sec stores min and hour also */
};

typedef struct _xmlSchemaValDecimal xmlSchemaValDecimal;
typedef xmlSchemaValDecimal *xmlSchemaValDecimalPtr;
struct _xmlSchemaValDecimal {
    /* would use long long but not portable */
    unsigned long lo;
    unsigned long mi;
    unsigned long hi;
    unsigned int extra;
    unsigned int sign:1;
    unsigned int frac:7;
    unsigned int total:8;
};

typedef struct _xmlSchemaValQName xmlSchemaValQName;
typedef xmlSchemaValQName *xmlSchemaValQNamePtr;
struct _xmlSchemaValQName {
    xmlChar *name;
    xmlChar *uri;
};

typedef struct _xmlSchemaValHex xmlSchemaValHex;
typedef xmlSchemaValHex *xmlSchemaValHexPtr;
struct _xmlSchemaValHex {
    xmlChar     *str;
    unsigned int total;
};

typedef struct _xmlSchemaValBase64 xmlSchemaValBase64;
typedef xmlSchemaValBase64 *xmlSchemaValBase64Ptr;
struct _xmlSchemaValBase64 {
    xmlChar     *str;
    unsigned int total;
};

struct _xmlSchemaVal {
    xmlSchemaValType type;
    union {
	xmlSchemaValDecimal     decimal;
        xmlSchemaValDate        date;
        xmlSchemaValDuration    dur;
	xmlSchemaValQName	qname;
	xmlSchemaValHex		hex;
	xmlSchemaValBase64	base64;
	float			f;
	double			d;
	int			b;
	xmlChar                *str;
    } value;
};

static int xmlSchemaTypesInitialized = 0;
static xmlHashTablePtr xmlSchemaTypesBank = NULL;

/*
 * Basic types
 */
static xmlSchemaTypePtr xmlSchemaTypeStringDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeAnyTypeDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeAnySimpleTypeDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDecimalDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDatetimeDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDateDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeTimeDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGYearDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGYearMonthDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGDayDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGMonthDayDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGMonthDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDurationDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeFloatDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeBooleanDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDoubleDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeHexBinaryDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeBase64BinaryDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeAnyURIDef = NULL;

/*
 * Derived types
 */
static xmlSchemaTypePtr xmlSchemaTypePositiveIntegerDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNonPositiveIntegerDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNegativeIntegerDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNonNegativeIntegerDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeIntegerDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeLongDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeIntDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeShortDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeByteDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeUnsignedLongDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeUnsignedIntDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeUnsignedShortDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeUnsignedByteDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNormStringDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeTokenDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeLanguageDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNameDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeQNameDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNCNameDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeIdDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeIdrefDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeIdrefsDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeEntityDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeEntitiesDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNotationDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNmtokenDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNmtokensDef = NULL;

/************************************************************************
 *									*
 * 			Datatype error handlers				*
 *									*
 ************************************************************************/
/**
 * xmlSchemaTypeErrMemory:
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlSchemaTypeErrMemory(xmlNodePtr node, const char *extra)
{
    __xmlSimpleError(XML_FROM_DATATYPE, XML_ERR_NO_MEMORY, node, NULL, extra);
}

/************************************************************************
 *									*
 * 			Base types support				*
 *									*
 ************************************************************************/
/*
 * xmlSchemaInitBasicType:
 * @name:  the type name
 * @type:  the value type associated
 *
 * Initialize one primitive built-in type
 */
static xmlSchemaTypePtr
xmlSchemaInitBasicType(const char *name, xmlSchemaValType type, 
		       xmlSchemaTypePtr baseType) {
    xmlSchemaTypePtr ret;

    ret = (xmlSchemaTypePtr) xmlMalloc(sizeof(xmlSchemaType));
    if (ret == NULL) {
        xmlSchemaTypeErrMemory(NULL, "could not initialize basic types");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaType));
    ret->name = (const xmlChar *)name;
    ret->type = XML_SCHEMA_TYPE_BASIC;
    ret->baseType = baseType;
    /*
    * Hack to reflect the variety.
    */
    if ((type == XML_SCHEMAS_IDREFS) ||
	(type == XML_SCHEMAS_NMTOKENS) ||
	(type == XML_SCHEMAS_ENTITIES)) 
	ret->flags |= XML_SCHEMAS_TYPE_VARIETY_LIST;
    else if ((type != XML_SCHEMAS_ANYTYPE) &&
	(type != XML_SCHEMAS_ANYSIMPLETYPE))
	ret->flags |= XML_SCHEMAS_TYPE_VARIETY_ATOMIC;
    ret->contentType = XML_SCHEMA_CONTENT_BASIC;
    switch (type) {
	case XML_SCHEMAS_STRING:            
	case XML_SCHEMAS_DECIMAL:    
	case XML_SCHEMAS_DATE:    
	case XML_SCHEMAS_DATETIME:    
	case XML_SCHEMAS_TIME:    
	case XML_SCHEMAS_GYEAR:    
	case XML_SCHEMAS_GYEARMONTH:    
	case XML_SCHEMAS_GMONTH:    
	case XML_SCHEMAS_GMONTHDAY:    
	case XML_SCHEMAS_GDAY:    
	case XML_SCHEMAS_DURATION:    
	case XML_SCHEMAS_FLOAT:    
	case XML_SCHEMAS_DOUBLE:    
	case XML_SCHEMAS_BOOLEAN:    
	case XML_SCHEMAS_ANYURI:    
	case XML_SCHEMAS_HEXBINARY:    
	case XML_SCHEMAS_BASE64BINARY:	
	case XML_SCHEMAS_QNAME:	
	case XML_SCHEMAS_NOTATION:	
	    ret->flags |= XML_SCHEMAS_TYPE_BUILTIN_PRIMITIVE;
	default:
	break;
    }

    xmlHashAddEntry2(xmlSchemaTypesBank, ret->name,
	             XML_SCHEMAS_NAMESPACE_NAME, ret);
    ret->builtInType = type;
    return(ret);
}

/*
 * xmlSchemaInitTypes:
 *
 * Initialize the default XML Schemas type library
 */
void
xmlSchemaInitTypes(void)
{
    if (xmlSchemaTypesInitialized != 0)
        return;
    xmlSchemaTypesBank = xmlHashCreate(40);

    
    /*
    * 3.4.7 Built-in Complex Type Definition
    */
    xmlSchemaTypeAnyTypeDef = xmlSchemaInitBasicType("anyType",
                                                     XML_SCHEMAS_ANYTYPE, 
						     NULL);
    xmlSchemaTypeAnyTypeDef->baseType = xmlSchemaTypeAnyTypeDef;
    xmlSchemaTypeAnyTypeDef->contentType = XML_SCHEMA_CONTENT_MIXED;
    {
	xmlSchemaWildcardPtr wild;

	wild = (xmlSchemaWildcardPtr) xmlMalloc(sizeof(xmlSchemaWildcard));
	if (wild == NULL) {
	    xmlSchemaTypeErrMemory(NULL, "could not create an attribute wildcard on anyType");
	    return;
	}
	memset(wild, 0, sizeof(xmlSchemaWildcard));
	wild->any = 1;
	wild->processContents = XML_SCHEMAS_ANY_LAX;
	wild->minOccurs = 1;
	wild->maxOccurs = 1;
	xmlSchemaTypeAnyTypeDef->attributeWildcard = wild;
    }
    xmlSchemaTypeAnySimpleTypeDef = xmlSchemaInitBasicType("anySimpleType", 
                                                           XML_SCHEMAS_ANYSIMPLETYPE,
							   xmlSchemaTypeAnyTypeDef);
    /*
    * primitive datatypes
    */
    xmlSchemaTypeStringDef = xmlSchemaInitBasicType("string",
                                                    XML_SCHEMAS_STRING,
						    xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeDecimalDef = xmlSchemaInitBasicType("decimal",
                                                     XML_SCHEMAS_DECIMAL,
						     xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeDateDef = xmlSchemaInitBasicType("date",
                                                  XML_SCHEMAS_DATE,
						  xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeDatetimeDef = xmlSchemaInitBasicType("dateTime",
                                                      XML_SCHEMAS_DATETIME,
						      xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeTimeDef = xmlSchemaInitBasicType("time",
                                                  XML_SCHEMAS_TIME,
						  xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeGYearDef = xmlSchemaInitBasicType("gYear",
                                                   XML_SCHEMAS_GYEAR,
						   xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeGYearMonthDef = xmlSchemaInitBasicType("gYearMonth",
                                                        XML_SCHEMAS_GYEARMONTH,
							xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeGMonthDef = xmlSchemaInitBasicType("gMonth",
                                                    XML_SCHEMAS_GMONTH,
						    xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeGMonthDayDef = xmlSchemaInitBasicType("gMonthDay",
                                                       XML_SCHEMAS_GMONTHDAY,
						       xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeGDayDef = xmlSchemaInitBasicType("gDay",
                                                  XML_SCHEMAS_GDAY,
						  xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeDurationDef = xmlSchemaInitBasicType("duration",
                                                      XML_SCHEMAS_DURATION,
						      xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeFloatDef = xmlSchemaInitBasicType("float",
                                                   XML_SCHEMAS_FLOAT,
						   xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeDoubleDef = xmlSchemaInitBasicType("double",
                                                    XML_SCHEMAS_DOUBLE,
						    xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeBooleanDef = xmlSchemaInitBasicType("boolean",
                                                     XML_SCHEMAS_BOOLEAN,
						     xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeAnyURIDef = xmlSchemaInitBasicType("anyURI",
                                                    XML_SCHEMAS_ANYURI,
						    xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeHexBinaryDef = xmlSchemaInitBasicType("hexBinary",
                                                     XML_SCHEMAS_HEXBINARY,
						     xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeBase64BinaryDef
        = xmlSchemaInitBasicType("base64Binary", XML_SCHEMAS_BASE64BINARY,
	xmlSchemaTypeAnySimpleTypeDef);
    xmlSchemaTypeNotationDef = xmlSchemaInitBasicType("NOTATION",
                                                    XML_SCHEMAS_NOTATION,
						    xmlSchemaTypeAnySimpleTypeDef);    
    xmlSchemaTypeQNameDef = xmlSchemaInitBasicType("QName",
                                                   XML_SCHEMAS_QNAME,
						   xmlSchemaTypeAnySimpleTypeDef);

    /*
     * derived datatypes
     */
    xmlSchemaTypeIntegerDef = xmlSchemaInitBasicType("integer",
                                                     XML_SCHEMAS_INTEGER,
						     xmlSchemaTypeDecimalDef);
    xmlSchemaTypeNonPositiveIntegerDef =
        xmlSchemaInitBasicType("nonPositiveInteger",
                               XML_SCHEMAS_NPINTEGER,
			       xmlSchemaTypeIntegerDef);
    xmlSchemaTypeNegativeIntegerDef =
        xmlSchemaInitBasicType("negativeInteger", XML_SCHEMAS_NINTEGER,
	xmlSchemaTypeNonPositiveIntegerDef);
    xmlSchemaTypeLongDef =
        xmlSchemaInitBasicType("long", XML_SCHEMAS_LONG,
	xmlSchemaTypeIntegerDef);
    xmlSchemaTypeIntDef = xmlSchemaInitBasicType("int", XML_SCHEMAS_INT,
	xmlSchemaTypeLongDef);
    xmlSchemaTypeShortDef = xmlSchemaInitBasicType("short",
                                                   XML_SCHEMAS_SHORT,
						   xmlSchemaTypeIntDef);
    xmlSchemaTypeByteDef = xmlSchemaInitBasicType("byte",
                                                  XML_SCHEMAS_BYTE,
						  xmlSchemaTypeShortDef);
    xmlSchemaTypeNonNegativeIntegerDef =
        xmlSchemaInitBasicType("nonNegativeInteger",
                               XML_SCHEMAS_NNINTEGER,
			       xmlSchemaTypeIntegerDef);
    xmlSchemaTypeUnsignedLongDef =
        xmlSchemaInitBasicType("unsignedLong", XML_SCHEMAS_ULONG,
	xmlSchemaTypeNonNegativeIntegerDef);
    xmlSchemaTypeUnsignedIntDef =
        xmlSchemaInitBasicType("unsignedInt", XML_SCHEMAS_UINT,
	xmlSchemaTypeUnsignedLongDef);
    xmlSchemaTypeUnsignedShortDef =
        xmlSchemaInitBasicType("unsignedShort", XML_SCHEMAS_USHORT,
	xmlSchemaTypeUnsignedIntDef);
    xmlSchemaTypeUnsignedByteDef =
        xmlSchemaInitBasicType("unsignedByte", XML_SCHEMAS_UBYTE,
	xmlSchemaTypeUnsignedShortDef);
    xmlSchemaTypePositiveIntegerDef =
        xmlSchemaInitBasicType("positiveInteger", XML_SCHEMAS_PINTEGER,
	xmlSchemaTypeNonNegativeIntegerDef);
    xmlSchemaTypeNormStringDef = xmlSchemaInitBasicType("normalizedString",
                                                        XML_SCHEMAS_NORMSTRING,
							xmlSchemaTypeStringDef);
    xmlSchemaTypeTokenDef = xmlSchemaInitBasicType("token",
                                                   XML_SCHEMAS_TOKEN,
						   xmlSchemaTypeNormStringDef);
    xmlSchemaTypeLanguageDef = xmlSchemaInitBasicType("language",
                                                      XML_SCHEMAS_LANGUAGE,
						      xmlSchemaTypeTokenDef);
    xmlSchemaTypeNameDef = xmlSchemaInitBasicType("Name",
                                                  XML_SCHEMAS_NAME,
						  xmlSchemaTypeTokenDef);
    xmlSchemaTypeNmtokenDef = xmlSchemaInitBasicType("NMTOKEN",
                                                     XML_SCHEMAS_NMTOKEN,
						     xmlSchemaTypeTokenDef);                
    xmlSchemaTypeNCNameDef = xmlSchemaInitBasicType("NCName",
                                                    XML_SCHEMAS_NCNAME,
						    xmlSchemaTypeNameDef);
    xmlSchemaTypeIdDef = xmlSchemaInitBasicType("ID", XML_SCHEMAS_ID,
	xmlSchemaTypeNCNameDef);
    xmlSchemaTypeIdrefDef = xmlSchemaInitBasicType("IDREF",
                                                   XML_SCHEMAS_IDREF,
						   xmlSchemaTypeNCNameDef);
    xmlSchemaTypeIdrefsDef = xmlSchemaInitBasicType("IDREFS",
                                                    XML_SCHEMAS_IDREFS,
						    xmlSchemaTypeIdrefDef);    
    xmlSchemaTypeNmtokensDef = xmlSchemaInitBasicType("NMTOKENS",
                                                      XML_SCHEMAS_NMTOKENS,
						      xmlSchemaTypeNmtokenDef);
    xmlSchemaTypeEntityDef = xmlSchemaInitBasicType("ENTITY",
                                                    XML_SCHEMAS_ENTITY,
						    xmlSchemaTypeNCNameDef);
    xmlSchemaTypeEntitiesDef = xmlSchemaInitBasicType("ENTITIES",
                                                      XML_SCHEMAS_ENTITIES,
						      xmlSchemaTypeNCNameDef);
    xmlSchemaTypesInitialized = 1;
}

/**
 * xmlSchemaCleanupTypes:
 *
 * Cleanup the default XML Schemas type library
 */
void	
xmlSchemaCleanupTypes(void) {
    if (xmlSchemaTypesInitialized == 0)
	return;
    xmlSchemaFreeWildcard(xmlSchemaTypeAnyTypeDef->attributeWildcard);
    xmlHashFree(xmlSchemaTypesBank, (xmlHashDeallocator) xmlSchemaFreeType);
    xmlSchemaTypesInitialized = 0;
}

/**
 * xmlSchemaIsBuiltInTypeFacet:
 * @type: the built-in type
 * @facetType:  the facet type
 *
 * Evaluates if a specific facet can be
 * used in conjunction with a type.
 *
 * Returns 1 if the facet can be used with the given built-in type,
 * 0 otherwise and -1 in case the type is not a built-in type.
 */
int
xmlSchemaIsBuiltInTypeFacet(xmlSchemaTypePtr type, int facetType)
{
    if (type == NULL)
	return (-1);
    if (type->type != XML_SCHEMA_TYPE_BASIC)
	return (-1);
    switch (type->builtInType) {
	case XML_SCHEMAS_BOOLEAN:
	    if ((facetType == XML_SCHEMA_FACET_PATTERN) ||
		(facetType == XML_SCHEMA_FACET_WHITESPACE))
		return (1);
	    else
		return (0);	
	case XML_SCHEMAS_STRING:
	case XML_SCHEMAS_NOTATION:
	case XML_SCHEMAS_QNAME:
	case XML_SCHEMAS_ANYURI:	    
	case XML_SCHEMAS_BASE64BINARY:    
	case XML_SCHEMAS_HEXBINARY:
	    if ((facetType == XML_SCHEMA_FACET_LENGTH) ||
		(facetType == XML_SCHEMA_FACET_MINLENGTH) ||
		(facetType == XML_SCHEMA_FACET_MAXLENGTH) ||
		(facetType == XML_SCHEMA_FACET_PATTERN) ||
		(facetType == XML_SCHEMA_FACET_ENUMERATION) ||
		(facetType == XML_SCHEMA_FACET_WHITESPACE))
		return (1);
	    else
		return (0);
	case XML_SCHEMAS_DECIMAL:
	    if ((facetType == XML_SCHEMA_FACET_TOTALDIGITS) ||
		(facetType == XML_SCHEMA_FACET_FRACTIONDIGITS) ||
		(facetType == XML_SCHEMA_FACET_PATTERN) ||
		(facetType == XML_SCHEMA_FACET_WHITESPACE) ||
		(facetType == XML_SCHEMA_FACET_ENUMERATION) ||
		(facetType == XML_SCHEMA_FACET_MAXINCLUSIVE) ||
		(facetType == XML_SCHEMA_FACET_MAXEXCLUSIVE) ||
		(facetType == XML_SCHEMA_FACET_MININCLUSIVE) ||
		(facetType == XML_SCHEMA_FACET_MINEXCLUSIVE))
		return (1);
	    else
		return (0); 
	case XML_SCHEMAS_TIME:
	case XML_SCHEMAS_GDAY: 
	case XML_SCHEMAS_GMONTH:
	case XML_SCHEMAS_GMONTHDAY: 
	case XML_SCHEMAS_GYEAR: 
	case XML_SCHEMAS_GYEARMONTH:
	case XML_SCHEMAS_DATE:
	case XML_SCHEMAS_DATETIME:
	case XML_SCHEMAS_DURATION:
	case XML_SCHEMAS_FLOAT:
	case XML_SCHEMAS_DOUBLE:
	    if ((facetType == XML_SCHEMA_FACET_PATTERN) ||
		(facetType == XML_SCHEMA_FACET_ENUMERATION) ||
		(facetType == XML_SCHEMA_FACET_WHITESPACE) ||
		(facetType == XML_SCHEMA_FACET_MAXINCLUSIVE) ||
		(facetType == XML_SCHEMA_FACET_MAXEXCLUSIVE) ||
		(facetType == XML_SCHEMA_FACET_MININCLUSIVE) ||
		(facetType == XML_SCHEMA_FACET_MINEXCLUSIVE))
		return (1);
	    else
		return (0);	    				 
	default:
	    break;
    }
    return (0);
}

/**
 * xmlSchemaGetBuiltInType:
 * @type:  the type of the built in type
 *
 * Gives you the type struct for a built-in
 * type by its type id.
 *
 * Returns the type if found, NULL otherwise.
 */
xmlSchemaTypePtr
xmlSchemaGetBuiltInType(xmlSchemaValType type)
{
    if (xmlSchemaTypesInitialized == 0)
	xmlSchemaInitTypes();
    switch (type) {
	
	case XML_SCHEMAS_ANYSIMPLETYPE:
	    return (xmlSchemaTypeAnySimpleTypeDef);
	case XML_SCHEMAS_STRING:
	    return (xmlSchemaTypeStringDef);
	case XML_SCHEMAS_NORMSTRING:
	    return (xmlSchemaTypeNormStringDef);
	case XML_SCHEMAS_DECIMAL:
	    return (xmlSchemaTypeDecimalDef);
	case XML_SCHEMAS_TIME:
	    return (xmlSchemaTypeTimeDef);
	case XML_SCHEMAS_GDAY:
	    return (xmlSchemaTypeGDayDef);
	case XML_SCHEMAS_GMONTH:
	    return (xmlSchemaTypeGMonthDef);
	case XML_SCHEMAS_GMONTHDAY:
    	    return (xmlSchemaTypeGMonthDayDef);
	case XML_SCHEMAS_GYEAR:
	    return (xmlSchemaTypeGYearDef);
	case XML_SCHEMAS_GYEARMONTH:
	    return (xmlSchemaTypeGYearMonthDef);
	case XML_SCHEMAS_DATE:
	    return (xmlSchemaTypeDateDef);
	case XML_SCHEMAS_DATETIME:
	    return (xmlSchemaTypeDatetimeDef);
	case XML_SCHEMAS_DURATION:
	    return (xmlSchemaTypeDurationDef);
	case XML_SCHEMAS_FLOAT:
	    return (xmlSchemaTypeFloatDef);
	case XML_SCHEMAS_DOUBLE:
	    return (xmlSchemaTypeDoubleDef);
	case XML_SCHEMAS_BOOLEAN:
	    return (xmlSchemaTypeBooleanDef);
	case XML_SCHEMAS_TOKEN:
	    return (xmlSchemaTypeTokenDef);
	case XML_SCHEMAS_LANGUAGE:
	    return (xmlSchemaTypeLanguageDef);
	case XML_SCHEMAS_NMTOKEN:
	    return (xmlSchemaTypeNmtokenDef);
	case XML_SCHEMAS_NMTOKENS:
	    return (xmlSchemaTypeNmtokensDef);
	case XML_SCHEMAS_NAME:
	    return (xmlSchemaTypeNameDef);
	case XML_SCHEMAS_QNAME:
	    return (xmlSchemaTypeQNameDef);
	case XML_SCHEMAS_NCNAME:
	    return (xmlSchemaTypeNCNameDef);
	case XML_SCHEMAS_ID:
	    return (xmlSchemaTypeIdDef);
	case XML_SCHEMAS_IDREF:
	    return (xmlSchemaTypeIdrefDef);
	case XML_SCHEMAS_IDREFS:
	    return (xmlSchemaTypeIdrefsDef);
	case XML_SCHEMAS_ENTITY:
	    return (xmlSchemaTypeEntityDef);
	case XML_SCHEMAS_ENTITIES:
	    return (xmlSchemaTypeEntitiesDef);
	case XML_SCHEMAS_NOTATION:
	    return (xmlSchemaTypeNotationDef);
	case XML_SCHEMAS_ANYURI:
	    return (xmlSchemaTypeAnyURIDef);
	case XML_SCHEMAS_INTEGER:
	    return (xmlSchemaTypeIntegerDef);
	case XML_SCHEMAS_NPINTEGER:
	    return (xmlSchemaTypeNonPositiveIntegerDef);
	case XML_SCHEMAS_NINTEGER:
	    return (xmlSchemaTypeNegativeIntegerDef);
	case XML_SCHEMAS_NNINTEGER:
	    return (xmlSchemaTypeNonNegativeIntegerDef);
	case XML_SCHEMAS_PINTEGER:
	    return (xmlSchemaTypePositiveIntegerDef);
	case XML_SCHEMAS_INT:
	    return (xmlSchemaTypeIntDef);
	case XML_SCHEMAS_UINT:
	    return (xmlSchemaTypeUnsignedIntDef);
	case XML_SCHEMAS_LONG:
	    return (xmlSchemaTypeLongDef);
	case XML_SCHEMAS_ULONG:
	    return (xmlSchemaTypeUnsignedLongDef);
	case XML_SCHEMAS_SHORT:
	    return (xmlSchemaTypeShortDef);
	case XML_SCHEMAS_USHORT:
	    return (xmlSchemaTypeUnsignedShortDef);
	case XML_SCHEMAS_BYTE:
	    return (xmlSchemaTypeByteDef);
	case XML_SCHEMAS_UBYTE:
	    return (xmlSchemaTypeUnsignedByteDef);
	case XML_SCHEMAS_HEXBINARY:
	    return (xmlSchemaTypeHexBinaryDef);
	case XML_SCHEMAS_BASE64BINARY:
	    return (xmlSchemaTypeBase64BinaryDef);
	case XML_SCHEMAS_ANYTYPE:
	    return (xmlSchemaTypeAnyTypeDef);	    
	default:
	    return (NULL);
    }
}

/**
 * xmlSchemaNewValue:
 * @type:  the value type
 *
 * Allocate a new simple type value
 *
 * Returns a pointer to the new value or NULL in case of error
 */
static xmlSchemaValPtr
xmlSchemaNewValue(xmlSchemaValType type) {
    xmlSchemaValPtr value;

    value = (xmlSchemaValPtr) xmlMalloc(sizeof(xmlSchemaVal));
    if (value == NULL) {
	return(NULL);
    }
    memset(value, 0, sizeof(xmlSchemaVal));
    value->type = type;
    return(value);
}

/**
 * xmlSchemaFreeValue:
 * @value:  the value to free
 *
 * Cleanup the default XML Schemas type library
 */
void	
xmlSchemaFreeValue(xmlSchemaValPtr value) {
    if (value == NULL)
	return;
    switch (value->type) {
        case XML_SCHEMAS_STRING:
        case XML_SCHEMAS_NORMSTRING:
        case XML_SCHEMAS_TOKEN:
        case XML_SCHEMAS_LANGUAGE:
        case XML_SCHEMAS_NMTOKEN:
        case XML_SCHEMAS_NMTOKENS:
        case XML_SCHEMAS_NAME:
        case XML_SCHEMAS_NCNAME:
        case XML_SCHEMAS_ID:
        case XML_SCHEMAS_IDREF:
        case XML_SCHEMAS_IDREFS:
        case XML_SCHEMAS_ENTITY:
        case XML_SCHEMAS_ENTITIES:
        case XML_SCHEMAS_NOTATION:
        case XML_SCHEMAS_ANYURI:
	    if (value->value.str != NULL)
		xmlFree(value->value.str);
	    break;
        case XML_SCHEMAS_QNAME:
	    if (value->value.qname.uri != NULL)
		xmlFree(value->value.qname.uri);
	    if (value->value.qname.name != NULL)
		xmlFree(value->value.qname.name);
	    break;
        case XML_SCHEMAS_HEXBINARY:
	    if (value->value.hex.str != NULL)
		xmlFree(value->value.hex.str);
	    break;
        case XML_SCHEMAS_BASE64BINARY:
	    if (value->value.base64.str != NULL)
		xmlFree(value->value.base64.str);
	    break;
	default:
	    break;
    }
    xmlFree(value);
}

/**
 * xmlSchemaGetPredefinedType:
 * @name: the type name
 * @ns:  the URI of the namespace usually "http://www.w3.org/2001/XMLSchema"
 *
 * Lookup a type in the default XML Schemas type library
 *
 * Returns the type if found, NULL otherwise
 */
xmlSchemaTypePtr
xmlSchemaGetPredefinedType(const xmlChar *name, const xmlChar *ns) {
    if (xmlSchemaTypesInitialized == 0)
	xmlSchemaInitTypes();
    if (name == NULL)
	return(NULL);
    return((xmlSchemaTypePtr) xmlHashLookup2(xmlSchemaTypesBank, name, ns));
}

/**
 * xmlSchemaGetBuiltInListSimpleTypeItemType:
 * @type: the built-in simple type.
 *
 * Lookup function
 *
 * Returns the item type of @type as defined by the built-in datatype
 * hierarchy of XML Schema Part 2: Datatypes, or NULL in case of an error.
 */
xmlSchemaTypePtr
xmlSchemaGetBuiltInListSimpleTypeItemType(xmlSchemaTypePtr type)
{
    if ((type == NULL) || (type->type != XML_SCHEMA_TYPE_BASIC))
	return (NULL);
    switch (type->builtInType) {
	case XML_SCHEMAS_NMTOKENS: 
	    return (xmlSchemaTypeNmtokenDef );
	case XML_SCHEMAS_IDREFS: 
	    return (xmlSchemaTypeIdrefDef);
	case XML_SCHEMAS_ENTITIES:
	    return (xmlSchemaTypeEntityDef);
	default:
	    return (NULL);
    }
}

/****************************************************************
 *								*
 *		Convenience macros and functions		*
 *								*
 ****************************************************************/

#define IS_TZO_CHAR(c)						\
	((c == 0) || (c == 'Z') || (c == '+') || (c == '-'))

#define VALID_YEAR(yr)          (yr != 0)
#define VALID_MONTH(mon)        ((mon >= 1) && (mon <= 12))
/* VALID_DAY should only be used when month is unknown */
#define VALID_DAY(day)          ((day >= 1) && (day <= 31))
#define VALID_HOUR(hr)          ((hr >= 0) && (hr <= 23))
#define VALID_MIN(min)          ((min >= 0) && (min <= 59))
#define VALID_SEC(sec)          ((sec >= 0) && (sec < 60))
#define VALID_TZO(tzo)          ((tzo > -1440) && (tzo < 1440))
#define IS_LEAP(y)						\
	(((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0))

static const unsigned int daysInMonth[12] =
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const unsigned int daysInMonthLeap[12] =
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#define MAX_DAYINMONTH(yr,mon)                                  \
        (IS_LEAP(yr) ? daysInMonthLeap[mon - 1] : daysInMonth[mon - 1])

#define VALID_MDAY(dt)						\
	(IS_LEAP(dt->year) ?				        \
	    (dt->day <= daysInMonthLeap[dt->mon - 1]) :	        \
	    (dt->day <= daysInMonth[dt->mon - 1]))

#define VALID_DATE(dt)						\
	(VALID_YEAR(dt->year) && VALID_MONTH(dt->mon) && VALID_MDAY(dt))

#define VALID_TIME(dt)						\
	(VALID_HOUR(dt->hour) && VALID_MIN(dt->min) &&		\
	 VALID_SEC(dt->sec) && VALID_TZO(dt->tzo))

#define VALID_DATETIME(dt)					\
	(VALID_DATE(dt) && VALID_TIME(dt))

#define SECS_PER_MIN            (60)
#define SECS_PER_HOUR           (60 * SECS_PER_MIN)
#define SECS_PER_DAY            (24 * SECS_PER_HOUR)

static const long dayInYearByMonth[12] =
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
static const long dayInLeapYearByMonth[12] =
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

#define DAY_IN_YEAR(day, month, year)				\
        ((IS_LEAP(year) ?					\
                dayInLeapYearByMonth[month - 1] :		\
                dayInYearByMonth[month - 1]) + day)

#ifdef DEBUG
#define DEBUG_DATE(dt)                                                  \
    xmlGenericError(xmlGenericErrorContext,                             \
        "type=%o %04ld-%02u-%02uT%02u:%02u:%03f",                       \
        dt->type,dt->value.date.year,dt->value.date.mon,                \
        dt->value.date.day,dt->value.date.hour,dt->value.date.min,      \
        dt->value.date.sec);                                            \
    if (dt->value.date.tz_flag)                                         \
        if (dt->value.date.tzo != 0)                                    \
            xmlGenericError(xmlGenericErrorContext,                     \
                "%+05d\n",dt->value.date.tzo);                          \
        else                                                            \
            xmlGenericError(xmlGenericErrorContext, "Z\n");             \
    else                                                                \
        xmlGenericError(xmlGenericErrorContext,"\n")
#else
#define DEBUG_DATE(dt)
#endif

/**
 * _xmlSchemaParseGYear:
 * @dt:  pointer to a date structure
 * @str: pointer to the string to analyze
 *
 * Parses a xs:gYear without time zone and fills in the appropriate
 * field of the @dt structure. @str is updated to point just after the
 * xs:gYear. It is supposed that @dt->year is big enough to contain
 * the year.
 *
 * Returns 0 or the error code
 */
static int
_xmlSchemaParseGYear (xmlSchemaValDatePtr dt, const xmlChar **str) {
    const xmlChar *cur = *str, *firstChar;
    int isneg = 0, digcnt = 0;

    if (((*cur < '0') || (*cur > '9')) &&
	(*cur != '-') && (*cur != '+'))
	return -1;

    if (*cur == '-') {
	isneg = 1;
	cur++;
    }

    firstChar = cur;

    while ((*cur >= '0') && (*cur <= '9')) {
	dt->year = dt->year * 10 + (*cur - '0');
	cur++;
	digcnt++;
    }

    /* year must be at least 4 digits (CCYY); over 4
     * digits cannot have a leading zero. */
    if ((digcnt < 4) || ((digcnt > 4) && (*firstChar == '0')))
	return 1;

    if (isneg)
	dt->year = - dt->year;

    if (!VALID_YEAR(dt->year))
	return 2;

    *str = cur;
    return 0;
}

/**
 * PARSE_2_DIGITS:
 * @num:  the integer to fill in
 * @cur:  an #xmlChar *
 * @invalid: an integer
 *
 * Parses a 2-digits integer and updates @num with the value. @cur is
 * updated to point just after the integer.
 * In case of error, @invalid is set to %TRUE, values of @num and
 * @cur are undefined.
 */
#define PARSE_2_DIGITS(num, cur, invalid)			\
	if ((cur[0] < '0') || (cur[0] > '9') ||			\
	    (cur[1] < '0') || (cur[1] > '9'))			\
	    invalid = 1;					\
	else							\
	    num = (cur[0] - '0') * 10 + (cur[1] - '0');		\
	cur += 2;

/**
 * PARSE_FLOAT:
 * @num:  the double to fill in
 * @cur:  an #xmlChar *
 * @invalid: an integer
 *
 * Parses a float and updates @num with the value. @cur is
 * updated to point just after the float. The float must have a
 * 2-digits integer part and may or may not have a decimal part.
 * In case of error, @invalid is set to %TRUE, values of @num and
 * @cur are undefined.
 */
#define PARSE_FLOAT(num, cur, invalid)				\
	PARSE_2_DIGITS(num, cur, invalid);			\
	if (!invalid && (*cur == '.')) {			\
	    double mult = 1;				        \
	    cur++;						\
	    if ((*cur < '0') || (*cur > '9'))			\
		invalid = 1;					\
	    while ((*cur >= '0') && (*cur <= '9')) {		\
		mult /= 10;					\
		num += (*cur - '0') * mult;			\
		cur++;						\
	    }							\
	}

/**
 * _xmlSchemaParseGMonth:
 * @dt:  pointer to a date structure
 * @str: pointer to the string to analyze
 *
 * Parses a xs:gMonth without time zone and fills in the appropriate
 * field of the @dt structure. @str is updated to point just after the
 * xs:gMonth.
 *
 * Returns 0 or the error code
 */
static int
_xmlSchemaParseGMonth (xmlSchemaValDatePtr dt, const xmlChar **str) {
    const xmlChar *cur = *str;
    int ret = 0;

    PARSE_2_DIGITS(dt->mon, cur, ret);
    if (ret != 0)
	return ret;

    if (!VALID_MONTH(dt->mon))
	return 2;

    *str = cur;
    return 0;
}

/**
 * _xmlSchemaParseGDay:
 * @dt:  pointer to a date structure
 * @str: pointer to the string to analyze
 *
 * Parses a xs:gDay without time zone and fills in the appropriate
 * field of the @dt structure. @str is updated to point just after the
 * xs:gDay.
 *
 * Returns 0 or the error code
 */
static int
_xmlSchemaParseGDay (xmlSchemaValDatePtr dt, 