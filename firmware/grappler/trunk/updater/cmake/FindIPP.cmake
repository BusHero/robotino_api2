IF( NOT IPP_FOUND )

MESSAGE( STATUS "Using IPP 7" )

ADD_DEFINITIONS( -DIPP_VERSION=7 )

SET( IPPARCH "intel64" )

SET(
	IPP_INCLUDE_PATHS
	"/opt/intel/ipp/include"
)

SET(
	IPP_LIBRARY_PATHS
	"/opt/intel/ipp/lib/${IPPARCH}"
)

FIND_PATH(
	IPP_INCLUDE_DIR
	ippi.h
	${IPP_INCLUDE_PATHS}
)

FIND_LIBRARY( IPP_IPPCORE_STATIC  NAMES ippcore_l PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPAC_STATIC    NAMES ippac_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCC_STATIC    NAMES ippcc_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCH_STATIC    NAMES ippch_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCP_STATIC    NAMES ippcp_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCV_STATIC    NAMES ippcv_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPDC_STATIC    NAMES ippdc_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPDI_STATIC    NAMES ippdi_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPGEN_STATIC   NAMES ippgen_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPI_STATIC     NAMES ippi_l    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPJ_STATIC     NAMES ippj_l    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPM_STATIC     NAMES ippm_l    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPR_STATIC     NAMES ippr_l    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPS_STATIC     NAMES ipps_l    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPSC_STATIC    NAMES ippsc_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPVC_STATIC    NAMES ippvc_l   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPVM_STATIC    NAMES ippvm_l   PATHS ${IPP_LIBRARY_PATHS} )

SET(
	IPP_STATIC_LIBRARIES
	${IPP_IPPAC_STATIC}
	${IPP_IPPCC_STATIC}
	${IPP_IPPCH_STATIC}
	${IPP_IPPCP_STATIC}
	${IPP_IPPCV_STATIC}
	${IPP_IPPDC_STATIC}
	${IPP_IPPDI_STATIC}
	${IPP_IPPGEN_STATIC}
	${IPP_IPPI_STATIC}
	${IPP_IPPJ_STATIC}
	${IPP_IPPM_STATIC}
	${IPP_IPPR_STATIC}
	${IPP_IPPS_STATIC}
	${IPP_IPPSC_STATIC}
	${IPP_IPPVC_STATIC}
	${IPP_IPPVM_STATIC}
	${IPP_IPPCORE_STATIC}
)

FIND_LIBRARY( IPP_IPPCORE_STATIC_MT  NAMES ippcore_t PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPAC_STATIC_MT    NAMES ippac_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCC_STATIC_MT    NAMES ippcc_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCH_STATIC_MT    NAMES ippch_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCP_STATIC_MT    NAMES ippcp_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCV_STATIC_MT    NAMES ippcv_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPDC_STATIC_MT    NAMES ippdc_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPDI_STATIC_MT    NAMES ippdi_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPGEN_STATIC_MT   NAMES ippgen_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPI_STATIC_MT     NAMES ippi_t    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPJ_STATIC_MT     NAMES ippj_t    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPM_STATIC_MT     NAMES ippm_t    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPR_STATIC_MT     NAMES ippr_t    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPS_STATIC_MT     NAMES ipps_t    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPSC_STATIC_MT    NAMES ippsc_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPVC_STATIC_MT    NAMES ippvc_t   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPVM_STATIC_MT    NAMES ippvm_t   PATHS ${IPP_LIBRARY_PATHS} )

SET(
	IPP_STATIC_MT_LIBRARIES
	${IPP_IPPAC_STATIC_MT}
	${IPP_IPPCC_STATIC_MT}
	${IPP_IPPCH_STATIC_MT}
	${IPP_IPPCP_STATIC_MT}
	${IPP_IPPCV_STATIC_MT}
	${IPP_IPPDC_STATIC_MT}
	${IPP_IPPDI_STATIC_MT}
	${IPP_IPPGEN_STATIC_MT}
	${IPP_IPPI_STATIC_MT}
	${IPP_IPPJ_STATIC_MT}
	${IPP_IPPM_STATIC_MT}
	${IPP_IPPR_STATIC_MT}
	${IPP_IPPS_STATIC_MT}
	${IPP_IPPSC_STATIC_MT}
	${IPP_IPPVC_STATIC_MT}
	${IPP_IPPVM_STATIC_MT}
	${IPP_IPPCORE_STATIC_MT}
)

FIND_LIBRARY( IPP_IPPCORE_SHARED NAMES    ippcore PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPAC_SHARED   NAMES    ippac   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCC_SHARED   NAMES    ippcc   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCH_SHARED   NAMES    ippch   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCP_SHARED   NAMES    ippcp   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPCV_SHARED   NAMES    ippcv   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPDC_SHARED   NAMES    ippdc   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPDI_SHARED   NAMES    ippdi   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPGEN_SHARED  NAMES    ippgen   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPI_SHARED    NAMES    ippi    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPJ_SHARED    NAMES    ippj    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPM_SHARED    NAMES    ippm    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPR_SHARED    NAMES    ippr    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPS_SHARED    NAMES    ipps    PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPSC_SHARED   NAMES    ippsc   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPVC_SHARED   NAMES    ippvc   PATHS ${IPP_LIBRARY_PATHS} )
FIND_LIBRARY( IPP_IPPVM_SHARED   NAMES    ippvm   PATHS ${IPP_LIBRARY_PATHS} )

SET(
	IPP_SHARED_LIBRARIES
	${IPP_IPPAC_SHARED}
	${IPP_IPPCC_SHARED}
	${IPP_IPPCH_SHARED}
	${IPP_IPPCP_SHARED}
	${IPP_IPPCORE_SHARED}
	${IPP_IPPCV_SHARED}
	${IPP_IPPDC_SHARED}
	${IPP_IPPDI_SHARED}
	${IPP_IPPGEN_SHARED}
	${IPP_IPPI_SHARED}
	${IPP_IPPJ_SHARED}
	${IPP_IPPM_SHARED}
	${IPP_IPPR_SHARED}
	${IPP_IPPS_SHARED}
	${IPP_IPPSC_SHARED}
	${IPP_IPPVC_SHARED}
	${IPP_IPPVM_SHARED}
)

SET( IPP_FOUND 0 )
IF( IPP_INCLUDE_DIR )
	SET( IPP_FOUND 1 )
	ADD_DEFINITIONS( -DHAVE_IPP )
	INCLUDE_DIRECTORIES(
		${IPP_INCLUDE_DIR}
	)
ENDIF( IPP_INCLUDE_DIR )

ENDIF( NOT IPP_FOUND )
