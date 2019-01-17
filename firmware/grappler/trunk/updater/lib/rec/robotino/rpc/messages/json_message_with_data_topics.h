#ifndef _REC_ROBOTINO_RPC_JSON_MESSAGE_WITH_DATA_TOPICS_H_
#define _REC_ROBOTINO_RPC_JSON_MESSAGE_WITH_DATA_TOPICS_H_

namespace rec
{
	namespace robotino
	{
		namespace rpc
		{
			typedef enum
			{
				//0-31 are normal topics
				smartNavigationPlanner_response_topic = 0,
				smartCdl_topic = 1,
				smartPersonTracker_topic = 2,
				fileSystem_request_topic = 3,
				fileSystem_response_topic = 4,
				//32-47 are permanent topic
				smartNavigationPlannner_topic = 32,
				ledd_topic = 33,
				customVis_topic = 34,
				mapDirWithData_topic = 35,
				smartNavigationCostMap_topic = 36,
				imageinfo_topic = 37,
				//48-63 are enqueued topics
				smartNavigationPlanner_request_topic = 48,
				iEasyLab_server_request_topic = 49,
				iEasyLab_server_response_topic = 50,

				json_message_with_data_topics_max = 64
			} json_message_with_data_topics;
		}
	}
}

#define rec_robotino_rpc_json_message_with_data_topic_0_name "smartNavigationPlanner_response"
#define rec_robotino_rpc_json_message_with_data_topic_1_name "smartCdl"
#define rec_robotino_rpc_json_message_with_data_topic_2_name "smartPersonTracker"
#define rec_robotino_rpc_json_message_with_data_topic_3_name "fileSystem_request"
#define rec_robotino_rpc_json_message_with_data_topic_4_name "fileSystem_response"
#define rec_robotino_rpc_json_message_with_data_topic_5_name ""
#define rec_robotino_rpc_json_message_with_data_topic_6_name ""
#define rec_robotino_rpc_json_message_with_data_topic_7_name ""
#define rec_robotino_rpc_json_message_with_data_topic_8_name ""
#define rec_robotino_rpc_json_message_with_data_topic_9_name ""
#define rec_robotino_rpc_json_message_with_data_topic_10_name ""
#define rec_robotino_rpc_json_message_with_data_topic_11_name ""
#define rec_robotino_rpc_json_message_with_data_topic_12_name ""
#define rec_robotino_rpc_json_message_with_data_topic_13_name ""
#define rec_robotino_rpc_json_message_with_data_topic_14_name ""
#define rec_robotino_rpc_json_message_with_data_topic_15_name ""
#define rec_robotino_rpc_json_message_with_data_topic_16_name ""
#define rec_robotino_rpc_json_message_with_data_topic_17_name ""
#define rec_robotino_rpc_json_message_with_data_topic_18_name ""
#define rec_robotino_rpc_json_message_with_data_topic_19_name ""
#define rec_robotino_rpc_json_message_with_data_topic_20_name ""
#define rec_robotino_rpc_json_message_with_data_topic_21_name ""
#define rec_robotino_rpc_json_message_with_data_topic_22_name ""
#define rec_robotino_rpc_json_message_with_data_topic_23_name ""
#define rec_robotino_rpc_json_message_with_data_topic_24_name ""
#define rec_robotino_rpc_json_message_with_data_topic_25_name ""
#define rec_robotino_rpc_json_message_with_data_topic_26_name ""
#define rec_robotino_rpc_json_message_with_data_topic_27_name ""
#define rec_robotino_rpc_json_message_with_data_topic_28_name ""
#define rec_robotino_rpc_json_message_with_data_topic_29_name ""
#define rec_robotino_rpc_json_message_with_data_topic_30_name ""
#define rec_robotino_rpc_json_message_with_data_topic_31_name ""
#define rec_robotino_rpc_json_message_with_data_topic_32_name "smartNavigationPlanner"
#define rec_robotino_rpc_json_message_with_data_topic_33_name "ledd"
#define rec_robotino_rpc_json_message_with_data_topic_34_name "customVis"
#define rec_robotino_rpc_json_message_with_data_topic_35_name "mapDirWithData"
#define rec_robotino_rpc_json_message_with_data_topic_36_name "smartNavigationCostMap"
#define rec_robotino_rpc_json_message_with_data_topic_37_name "imageinfo"
#define rec_robotino_rpc_json_message_with_data_topic_38_name ""
#define rec_robotino_rpc_json_message_with_data_topic_39_name ""
#define rec_robotino_rpc_json_message_with_data_topic_40_name ""
#define rec_robotino_rpc_json_message_with_data_topic_41_name ""
#define rec_robotino_rpc_json_message_with_data_topic_42_name ""
#define rec_robotino_rpc_json_message_with_data_topic_43_name ""
#define rec_robotino_rpc_json_message_with_data_topic_44_name ""
#define rec_robotino_rpc_json_message_with_data_topic_45_name ""
#define rec_robotino_rpc_json_message_with_data_topic_46_name ""
#define rec_robotino_rpc_json_message_with_data_topic_47_name ""
#define rec_robotino_rpc_json_message_with_data_topic_48_name "smartNavigationPlanner_request"
#define rec_robotino_rpc_json_message_with_data_topic_49_name "iEasyLab_server_request"
#define rec_robotino_rpc_json_message_with_data_topic_50_name "iEasyLab_server_response"
#define rec_robotino_rpc_json_message_with_data_topic_51_name ""
#define rec_robotino_rpc_json_message_with_data_topic_52_name ""
#define rec_robotino_rpc_json_message_with_data_topic_53_name ""
#define rec_robotino_rpc_json_message_with_data_topic_54_name ""
#define rec_robotino_rpc_json_message_with_data_topic_55_name ""
#define rec_robotino_rpc_json_message_with_data_topic_56_name ""
#define rec_robotino_rpc_json_message_with_data_topic_57_name ""
#define rec_robotino_rpc_json_message_with_data_topic_58_name ""
#define rec_robotino_rpc_json_message_with_data_topic_59_name ""
#define rec_robotino_rpc_json_message_with_data_topic_60_name ""
#define rec_robotino_rpc_json_message_with_data_topic_61_name ""
#define rec_robotino_rpc_json_message_with_data_topic_62_name ""
#define rec_robotino_rpc_json_message_with_data_topic_63_name ""


#endif //_REC_ROBOTINO_RPC_JSON_MESSAGE_WITH_DATA_TOPICS_H_
