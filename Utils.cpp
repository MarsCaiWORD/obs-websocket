#include "Utils.h"

obs_data_array_t* Utils::GetSceneItems(obs_source_t *source) {
	obs_data_array_t *items = obs_data_array_create();
	obs_scene_t *scene = obs_scene_from_source(source);
	if (scene == NULL) {
		return NULL;
	}

	obs_scene_enum_items(scene, [](obs_scene_t *scene, obs_sceneitem_t *currentItem, void *param) {
		obs_data_array_t *data = static_cast<obs_data_array_t *>(param);
		obs_data_array_push_back(data, GetSceneItemData(currentItem));
		return true;
	}, items);

	obs_scene_release(scene);
	return items;
}

obs_data_t* Utils::GetSceneItemData(obs_sceneitem_t *item) {
	if (!item) {
		return NULL;
	}

	vec2 pos;
	obs_sceneitem_get_pos(item, &pos);

	vec2 bounds;
	obs_sceneitem_get_bounds(item, &bounds);

	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "name", obs_source_get_name(obs_sceneitem_get_source(item)));
	obs_data_set_string(data, "type", obs_source_get_id(obs_sceneitem_get_source(item)));
	obs_data_set_double(data, "volume", obs_source_get_volume(obs_sceneitem_get_source(item)));
	obs_data_set_double(data, "x", pos.x);
	obs_data_set_double(data, "y", pos.y);
	obs_data_set_double(data, "cx", bounds.x);
	obs_data_set_double(data, "cy", bounds.y);
	obs_data_set_bool(data, "render", obs_sceneitem_visible(item));

	return data;
}


obs_sceneitem_t* Utils::GetSceneItemFromName(obs_source_t* source, const char* name) {
	struct current_search {
		const char* query;
		obs_sceneitem_t* result;
	};

	current_search search;
	search.query = name;
	search.result = NULL;

	obs_scene_t *scene = obs_scene_from_source(source);
	if (scene == NULL) {
		return NULL;
	}

	obs_scene_enum_items(scene, [](obs_scene_t *scene, obs_sceneitem_t *currentItem, void *param) {
		current_search *search = static_cast<current_search *>(param);
		
		const char* currentItemName = obs_source_get_name(obs_sceneitem_get_source(currentItem));
		if (strcmp(currentItemName, search->query) == 0) {
			search->result = currentItem;
			obs_sceneitem_addref(search->result);
			return false;
		}

		return true;
	}, &search);

	return search.result;
}

obs_data_array_t* Utils::GetScenes() {
	obs_frontend_source_list sceneList = {};
	obs_frontend_get_scenes(&sceneList);

	obs_data_array_t* scenes = obs_data_array_create();
	for (size_t i = 0; i < (&sceneList)->sources.num; i++) {
		obs_source_t* scene = (&sceneList)->sources.array[i];
		obs_data_array_push_back(scenes, GetSceneData(scene));
	}

	obs_frontend_source_list_free(&sceneList);

	return scenes;
}

obs_data_t* Utils::GetSceneData(obs_source *source) {
	obs_data_t* sceneData = obs_data_create();
	obs_data_set_string(sceneData, "name", obs_source_get_name(source));
	obs_data_set_array(sceneData, "sources", GetSceneItems(source));
	
	return sceneData;
}