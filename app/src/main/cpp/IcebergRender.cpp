#include "IcebergRender.h"

int64_t CreatePropInfoFromJsonPropInfo(UniUtil::ValType valType, const rj::Value& propJson, UniUtil::PropInfo& propInfo)
{
	//init space
	propInfo._keyframes.resize(propJson.Size());
	for (rj::SizeType idx = 0; idx < propJson.Size(); idx++)
	{
		propInfo._const._valType = valType;
		propInfo._keyframes[idx]._values.resize(propJson[idx]["values"].Size());
	}

	//set
	for (rj::SizeType idx = 0; idx < propJson.Size(); idx++)
	{
		for (rj::SizeType subIdx = 0; subIdx < propJson[idx]["values"].Size(); subIdx++)
		{
			switch (valType)
			{
			case UniUtil::ValType::Float1:
			{
				propInfo._keyframes[idx]._values[subIdx] = propJson[idx]["values"][subIdx][0].GetFloat();
				break;
			}
			case UniUtil::ValType::Float2:
			{
				glm::vec2 val = {
					propJson[idx]["values"][subIdx][0].GetFloat(),
					propJson[idx]["values"][subIdx][1].GetFloat(),
				};
				propInfo._keyframes[idx]._values[subIdx] = val;
				break;
			}
			case UniUtil::ValType::Float3:
			{
				glm::vec3 val = {
					propJson[idx]["values"][subIdx][0].GetFloat(),
					propJson[idx]["values"][subIdx][1].GetFloat(),
					propJson[idx]["values"][subIdx][2].GetFloat()
				};
				propInfo._keyframes[idx]._values[subIdx] = val;
				break;
			}
			case UniUtil::ValType::Float4:
			{
				glm::vec4 val = {
					propJson[idx]["values"][subIdx][0].GetFloat(),
					propJson[idx]["values"][subIdx][1].GetFloat(),
					propJson[idx]["values"][subIdx][2].GetFloat(),
					propJson[idx]["values"][subIdx][3].GetFloat()
				};
				propInfo._keyframes[idx]._values[subIdx] = val;
				break;
			}
			case UniUtil::ValType::Int1:
			{
				propInfo._keyframes[idx]._values[subIdx] = propJson[idx]["values"][subIdx][0].GetInt();
				break;
			}
			case UniUtil::ValType::Int2:
			{
				if (propJson[idx]["values"].IsArray())
				{
					int x = 42;
				}
				glm::ivec2 val = {
					propJson[idx]["values"][subIdx][0].GetInt(),
					propJson[idx]["values"][subIdx][1].GetInt()
				};
				propInfo._keyframes[idx]._values[subIdx] = val;
				break;
			}
			case UniUtil::ValType::Int3:
			{
				glm::ivec3 val = {
					propJson[idx]["values"][subIdx][0].GetInt(),
					propJson[idx]["values"][subIdx][1].GetInt(),
					propJson[idx]["values"][subIdx][2].GetInt()
				};
				propInfo._keyframes[idx]._values[subIdx] = val;
				break;
			}
			case UniUtil::ValType::Int4:
			{
				glm::ivec4 val = {
					propJson[idx]["values"][subIdx][0].GetInt(),
					propJson[idx]["values"][subIdx][1].GetInt(),
					propJson[idx]["values"][subIdx][2].GetInt(),
					propJson[idx]["values"][subIdx][3].GetInt()
				};
				propInfo._keyframes[idx]._values[subIdx] = val;
				break;
			}
			case UniUtil::ValType::Chars:
			{
				propInfo._keyframes[idx]._values[subIdx] = std::string(propJson[idx]["values"][subIdx][0].GetString());
				break;
			}
			case UniUtil::ValType::Bool:
			{
				propInfo._keyframes[idx]._values[subIdx] = propJson[idx]["values"][subIdx][0].GetBool();
				break;
			}
			default:
			{
				LOG(ERROR) << "Invalid type";
				break;
			}
			}
		}
	}

	return 0;
}

int64_t CreatePropInfoFromJsonPropHandleArray(UniUtil::TypeInfo typeInfo, const rj::Value& jsonPropHandleArray, UniUtil::PropInfo& propInfo)
{
	//init space
	propInfo._keyframes.resize(1);
	propInfo._const._valType = typeInfo._valType;
	propInfo._keyframes[UniUtil::IdxAt0]._values.resize(jsonPropHandleArray.Size());

	for (rj::SizeType idx = 0; idx < jsonPropHandleArray.Size(); idx++)
	{
		UniUtil::PropInfo prop;
		CreatePropInfoFromJsonPropInfo(UniUtil::ValType::Chars, jsonPropHandleArray[idx], prop);
		propInfo._keyframes[UniUtil::IdxAt0]._values[idx] = prop._keyframes[UniUtil::IdxAt0]._values[UniUtil::IdxAt0];
	}

	return 0;
}

int64_t IcebergRender::Init()
{
	Util::Log::Instance().Init(LogName, LogBase, LogInfo, LogWarning, LogErr, LogFatal);

	{
		proj.Init();

		proj.Load("ExportMaterial");
		proj.Save("ReExportMaterial");
	}

	std::map<UniUtil::Handle, BLayer::Sp_Layer> bLayers;
	std::map<UniUtil::Handle, BComp::Sp_Component> bComps;
	std::map<UniUtil::Handle, BMtl::Sp_Effect> bEffects;
	std::map<UniUtil::Handle, BMtl::Sp_Mask> bMasks;



	const std::string materialPath = "ExportMaterial";

	//LoadJson
	std::string content;
	if (!Util::LoadChar(materialPath + "/config.json", content)) {
		LOG(ERROR) << "Failed to load json file";
		return -1;
	}

	//allocate timeline
	rj::Document curProj;
	curProj.Parse(content.c_str());

	BComp::Sp_Composite lastComposite = nullptr;

	rj::Value& componentArray = curProj["Component"];
	std::map<Util::Handle, BComp::Sp_Component> componentMap;

	for (int64_t idx = 0; idx < static_cast<int64_t>(componentArray.Size()); idx++)
	{
		const rj::Value& currComp = componentArray[idx];

		std::string componentUUID = currComp["Common"]["UUID"][UniUtil::IdxAt0]["values"][UniUtil::IdxAt0][UniUtil::IdxAt0].GetString();

		if (currComp["ComponentType"][0]["values"][0][0] == "Comp")
		{
			//1-Create a general composite
			BComp::Sp_Composite composite(new BComp::Composite(componentUUID));
			composite->Init();

			bComps.insert({ composite->GetHandle(), composite });

			lastComposite = composite;

			componentMap.insert({ composite->GetHandle(), composite });

			//PropComponentGeneralMap
			for (const auto& item : UniUtil::PropComponentGeneralMap)
			{
				UniUtil::PropInfo propInfo;
				CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Component"][item.second._name.c_str()], propInfo);

				composite->AltPropComponentGeneral(item.first, propInfo);
			}

			//PropCompositeGeneralMap
			for (const auto& item : UniUtil::PropCompositeGeneralMap)
			{
				UniUtil::PropInfo propInfo;
				CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"][item.second._name.c_str()], propInfo);

				composite->AltPropCompositeGeneral(item.first, propInfo);
			}

			//PropCompositeGeneralBlockMap
			for (const auto& item : UniUtil::PropCompositeGeneralBlockMap)
			{
				UniUtil::PropInfo propInfo;
				CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"][item.second._name.c_str()], propInfo);

				for (size_t idx = 0; idx < propInfo._keyframes[0]._values.size(); idx++)
				{
					composite->AddPropCompositeGeneralBlock(item.first, std::get<std::string>(propInfo._keyframes[0]._values[idx]), idx);
				}

			}

			//layer process
			int64_t layerNum = 0;
			composite->GetLayerNum(layerNum);
			for (int64_t subIdx = 0; subIdx < layerNum; subIdx++)
			{
				BLayer::Sp_Layer layer = nullptr;

				std::string layerUUID = currComp["Composite"]["Layers"][subIdx]["Common"]["UUID"][0]["values"][0][0].GetString();


				UniUtil::PropInfo prop;
				CreatePropInfoFromJsonPropInfo(UniUtil::ValType::Int1, currComp["Composite"]["Layers"][subIdx]["LayerType"], prop);
				UniUtil::ComponentType layerType = static_cast<UniUtil::ComponentType>(std::get<int>(prop._keyframes[UniUtil::IdxAt0]._values[UniUtil::IdxAt0]));
				switch (layerType)
				{
				case UniUtil::ComponentType::Comp:
				{
					BLayer::Sp_LayerComp layerComp = std::make_shared<BLayer::LayerComp>(layerUUID);
					layerComp->Init();

					for (const auto& item : UniUtil::PropLayerCompGeneralMap)
					{
						UniUtil::PropInfo propInfo;
						CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"]["Layers"][subIdx]["LayerComp"][item.second._name.c_str()], propInfo);
						layerComp->AltProp(item.first, propInfo);
					}

					layer = layerComp;

					break;
				}
				case UniUtil::ComponentType::Image:
				{
					BLayer::Sp_LayerImage layerImage = std::make_shared<BLayer::LayerImage>(layerUUID);
					layerImage->Init();

					for (const auto& item : UniUtil::PropLayerImageGeneralMap)
					{
						UniUtil::PropInfo propInfo;
						CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"]["Layers"][subIdx]["LayerImage"][item.second._name.c_str()], propInfo);
						layerImage->AltProp(item.first, propInfo);
					}

					layer = layerImage;

					break;
				}
				}

				bLayers.insert({ layer->GetHandle(), layer });

				//fill layer data
				for (const auto& item : UniUtil::PropLayerGeneralMap)
				{
					UniUtil::PropInfo propInfo;
					CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"]["Layers"][subIdx]["General"][item.second._name.c_str()], propInfo);
					layer->AltProp(item.first, propInfo);
				}

				for (const auto& item : UniUtil::PropLayerTransformMap)
				{
					UniUtil::PropInfo propInfo;
					CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"]["Layers"][subIdx]["Transform"][item.second._name.c_str()], propInfo);
					layer->AltProp(item.first, propInfo);
				}

				for (const auto& item : UniUtil::PropLayerTransferModeMap)
				{
					UniUtil::PropInfo propInfo;
					CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"]["Layers"][subIdx]["TransferMode"][item.second._name.c_str()], propInfo);
					layer->AltProp(item.first, propInfo);
				}

				for (const auto& item : UniUtil::PropLayerTimeMap)
				{
					UniUtil::PropInfo propInfo;
					CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"]["Layers"][subIdx]["TimeZone"][item.second._name.c_str()], propInfo);
					layer->AltProp(item.first, propInfo);
				}

				for (const auto& item : UniUtil::PropLayerGeneralBlockMap)
				{
					UniUtil::PropInfo propInfo;
					CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Composite"]["Layers"][subIdx][item.second._name.c_str()], propInfo);

					for (size_t idx = 0; idx < propInfo._keyframes[0]._values.size(); idx++)
					{
						layer->AddPropLayerGeneralBlock(item.first, std::get<std::string>(propInfo._keyframes[0]._values[idx]), idx);
					}
				}

				for (rj::SizeType blockSubIdx = 0; blockSubIdx < currComp["Composite"]["Layers"][subIdx]["Effects"].Size(); blockSubIdx++)
				{
					const rj::Value& currEffectJson = currComp["Composite"]["Layers"][subIdx]["Effects"][blockSubIdx];

					std::string layerUUID = currComp["Composite"]["Layers"][subIdx]["Effects"][blockSubIdx]["Common"]["UUID"][0]["values"][0][0].GetString();

					BMtl::Sp_Effect effect = std::make_shared<BMtl::Effect>(layerUUID);
					effect->Init();

					bEffects.insert({ effect->GetHandle(), effect });

					for (const auto& effectItem : UniUtil::PropEffectGeneralMap)
					{
						UniUtil::PropInfo propInfo;
						CreatePropInfoFromJsonPropInfo(effectItem.second._valType, currEffectJson["General"][effectItem.second._name.c_str()], propInfo);

						effect->AltPropEffectGeneral(effectItem.first, propInfo);
					}

					for (rj::Value::ConstMemberIterator iter = currEffectJson["General"].MemberBegin(); iter != currEffectJson["General"].MemberEnd(); iter++)
					{
						UniUtil::PropInfo propInfo;
						CreatePropInfoFromJsonPropInfo(static_cast<UniUtil::ValType>(iter->value[0]["Type"][0].GetInt64()), iter->value, propInfo);

						effect->AddPropEffectParameter(iter->name.GetString(), propInfo);
					}

				}


				for (rj::SizeType blockSubIdx = 0; blockSubIdx < currComp["Composite"]["Layers"][subIdx]["Masks"].Size(); blockSubIdx++)
				{
					std::string layerUUID = currComp["Composite"]["Layers"][subIdx]["Masks"][blockSubIdx]["Common"]["UUID"][0]["values"][0][0].GetString();

					BMtl::Sp_Mask mask = std::make_shared<BMtl::Mask>(layerUUID);
					mask->Init();

					bMasks.insert({ mask->GetHandle(), mask });

					for (const auto& maskItem : UniUtil::PropMaskGeneralMap)
					{
						UniUtil::PropInfo propInfo;
						CreatePropInfoFromJsonPropInfo(maskItem.second._valType, currComp["Composite"]["Layers"][subIdx]["Masks"][blockSubIdx]["General"][maskItem.second._name.c_str()], propInfo);

						mask->AllocPropMaskParameter(maskItem.second._name, propInfo);
						mask->AltPropMaskGeneral(maskItem.first, propInfo);
					}

				}
			}
		}
		else if (currComp["ComponentType"][0]["values"][0][0] == "Image")
		{
			//create footageImage
			BComp::Sp_FootageImage ftg(new BComp::FootageImage(componentUUID));
			ftg->Init();
			bComps.insert({ ftg->GetHandle(), ftg });

			componentMap.insert({ ftg->GetHandle(), ftg });

			//PropComponentGeneralMap
			for (const auto& item : UniUtil::PropComponentGeneralMap)
			{
				UniUtil::PropInfo propInfo;
				CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Component"][item.second._name.c_str()], propInfo);

				ftg->AltPropComponentGeneral(item.first, propInfo);
			}

			//PropFootageGeneralMap
			for (const auto& item : UniUtil::PropFootageGeneralMap)
			{
				UniUtil::PropInfo propInfo;
				CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["Footage"][item.second._name.c_str()], propInfo);

				ftg->AltPropFootageGeneral(item.first, propInfo);
			}

			for (const auto& item : UniUtil::PropFootageImageGeneralMap)
			{
				UniUtil::PropInfo propInfo;
				CreatePropInfoFromJsonPropInfo(item.second._valType, currComp["FootageImage"][item.second._name.c_str()], propInfo);

				ftg->AltPropFootageImageGeneral(item.first, propInfo);
			}


			//load only footageImage data
		}
		else if (currComp["ComponentType"][0]["values"][0][0] == "Audio")
		{
			//create footageAudio

			//load component data

			//load only footageAudio data
		}
		else if (currComp["ComponentType"][0]["values"][0][0] == "Video")
		{
			//create footageVideo

			//load component data

			//load only footageVideo data
		}
		else if (currComp["ComponentType"][0]["values"][0][0] == "Text")
		{
			//create footageText
		}
	}


	proj.SetMainHandleH(lastComposite->GetHandle());
	proj.SetCurrHandleH(lastComposite->GetHandle());

	//ExportJson
	BExporter::Sp_Exporter exporter(new BExporter::Exporter(EAPI::OpType::Draft));
	exporter->SetExportHandle(proj.GetCurrHandleH());
	exporter->SetOriginResDir(proj.GetLoadDir());
	exporter->SetOutputPath(proj.GetSaveDir());
	exporter->DoExport();

	//Player
	player = std::make_shared<BPlayer::Player>(EAPI::OpType::PlayRender);
	player->Init();
	player->SetPlayHandle(proj.GetCurrHandleH());
	player->SetMaterialPackDir(proj.GetLoadDir());
	player->SetInternalResDir(InternalResDir);

	return 0;
}

int64_t IcebergRender::RenderLoop()
{

	player->DoPlay();

	return 0;

}
