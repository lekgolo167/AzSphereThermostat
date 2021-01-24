#include "msg.h"


void init_string(struct string *s) {
	s->len = 0;
	s->ptr = malloc(s->len + 1);
	if (s->ptr == NULL) {
		Log_Debug("malloc() failed\n");
		return;
	}
	s->ptr[0] = '\0';
	return;
};

size_t writedata(void *ptr, size_t size, size_t nmemb, struct string *s) {
	size_t new_len = s->len + size * nmemb;
	s->ptr = realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		Log_Debug("realloc() failed\n");
		return;
	}
	memcpy(s->ptr + s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;
	return size * nmemb;
};

bool getDayIDs(int* idArr) {
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		struct string s;
		init_string(&s);

		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
		curl_easy_setopt(curl, CURLOPT_URL, "192.168.0.27:5000/getDayIDs");

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			return false;
		}

		JSON_Value* raw = json_parse_string(s.ptr);
		JSON_Object* days = json_value_get_object(raw);
		char dates[7][4] = { "sun","mon","tue","wed","thu","fri","sat" };
		for (int i = 0; i < 7; i++) {
			Log_Debug("%s:, ID:%d\n", dates[i], (int)json_object_get_number(days, dates[i]));
			idArr[i] = (int)json_object_get_number(days, dates[i]);
		}
		json_value_free(raw);
		free(s.ptr);
		curl_easy_cleanup(curl);
		return true;
	}
	return false;
}

void getCycleData(int day, cycle_t * cycles_ptr) {
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		struct string s;
		init_string(&s);
		char url[30] = "192.168.0.27:5000/getCycles/x\0";
		url[28] = day + '0';
		Log_Debug("Getting cycles for: %d @ %s\n", day, url);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
		curl_easy_setopt(curl, CURLOPT_URL, url);

		res = curl_easy_perform(curl);

		//Log_Debug("%s\n", s.ptr);

		JSON_Value* raw = json_parse_string(s.ptr);
		JSON_Object* obj = json_value_get_object(raw);
		JSON_Array* cycles = json_object_get_array(obj, "cycles");

		Log_Debug("-===- Parsing -===-\n");

		for (int i = json_array_get_count(cycles) - 1; i >= 0; i--) {
			JSON_Object* cycle = json_array_get_object(cycles, i);

			Log_Debug("ID:%d\n\t%d:%d @%f F°\n", 
				(int)json_object_get_number(cycle, "id"),
				(int)json_object_get_number(cycle, "h"),
				(int)json_object_get_number(cycle, "m"),
				(float)json_object_get_number(cycle, "t"));

			push_end(cycles_ptr, (int)json_object_get_number(cycle, "id"),
				(int)json_object_get_number(cycle, "h"),
				(int)json_object_get_number(cycle, "m"),
				(float)json_object_get_number(cycle, "t"));
		}
		json_value_free(raw);
		free(s.ptr);

	}

	curl_easy_cleanup(curl);
	
	return;
}

void sendCURL(char* URLAndPath, char* dataFieldBuffer) {
	
	bool isNetworkingReady = false;
	if (!CURL_enabled || (Networking_IsNetworkingReady(&isNetworkingReady) < 0) || !isNetworkingReady) {
		Log_Debug("\nNot doing download because network is not up.\n");
		return;
	}
	CURL *curlHandle = NULL;
	CURLcode res = 0;
	Log_Debug("\n -===- Starting upload -===-\n");

	// Init the cURL library.
	if ((res = curl_global_init(CURL_GLOBAL_ALL)) != CURLE_OK) {
		Log_Debug("curl_global_init");
		return;
	}

	if ((curlHandle = curl_easy_init()) == NULL) {
		Log_Debug("curl_easy_init() failed\n");
		goto cleanupLabel;
	}

	// Set timeout
	if ((res = curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 10)) != CURLE_OK) {
		Log_Debug("curl_easy_setopt CURLOPT_URL");
		goto cleanupLabel;
	}

	// Specify URL to download.
	// Important: any change in the domain name must be reflected in the AllowedConnections
	// capability in app_manifest.json.
	if ((res = curl_easy_setopt(curlHandle, CURLOPT_URL, URLAndPath)) != CURLE_OK) {
		Log_Debug("curl_easy_setopt CURLOPT_URL");
		goto cleanupLabel;
	}

	// Set the POST data 
	if ((res = curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, dataFieldBuffer)) != CURLE_OK) {
		Log_Debug("curl_easy_setopt CURLOPT_VERBOSE");
		goto cleanupLabel;
	}

	// Perform the download of the web page.
	if ((res = curl_easy_perform(curlHandle)) != CURLE_OK) {
		Log_Debug(" -===- FAILED -===- \n");
	}

cleanupLabel:
	curl_easy_cleanup(curlHandle);
	// Clean up cURL library's resources.
	curl_global_cleanup();

	return;
}
