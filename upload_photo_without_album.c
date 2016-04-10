#include <gtk/gtk.h>
#include <gdata/gdata.h>
#include "gdata/gdata-service.h"
#include "gdata/gdata-client-login-authorizer.h"
#include <gdata/gdata-authorizer.h>
#include <gdata/services/picasaweb/gdata-picasaweb-service.h>
#include <gdata/gdata-goa-authorizer.h>
#include <gio/gio.h>

int main()
{
	GoaObject *goa_object;	
	GDataPicasaWebService *service;
	GDataGoaAuthorizer *authorizer;
	GError *error;
	GoaClient *client;
	GList *accounts_list, *l;

	GDataPicasaWebFile *file_entry, *uploaded_file_entry;
	GDataUploadStream *upload_stream;
	GFile *file_data;
	GFileInfo *file_info;
	GFileInputStream *file_stream;

	error = NULL;
	/*I am using sync version just to avoid some complexity and keeping it simple.*/
	client = goa_client_new_sync (NULL, &error);
	if (error != NULL)
	{
		printf("Cannot create goa client\n");
		return -1;
	}
	/*FYI, I have only experimented this with just one google account so account_lists assume 1 account only.*/
	accounts_list = goa_client_get_accounts (client);
	printf ("Accounts : %d\n", g_list_length (accounts_list));

	goa_object = accounts_list->data;
	authorizer = gdata_goa_authorizer_new (goa_object);
	service = gdata_picasaweb_service_new (GDATA_AUTHORIZER (authorizer));
	if (!gdata_service_is_authorized (GDATA_SERVICE (service)))
	{
		printf("Service not authorized\n");
		return -1;
	}
	/* Specify the GFile image on disk to upload */
	file_data = g_file_new_for_path ("/home/uajain/Downloads/cat1.jpg");

	/* Get the file information for the file being uploaded. If another data source was being used for the upload, it would have to
	 * provide an appropriate slug and content type. Note that this is a blocking operation. */
	file_info = g_file_query_info (file_data, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME "," G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
		                       G_FILE_QUERY_INFO_NONE, NULL, NULL);

	/* Create a GDataPicasaWebFile entry for the image, setting a title and caption/summary */
	file_entry = gdata_picasaweb_file_new (NULL);

	gdata_entry_set_title (GDATA_ENTRY (file_entry), "Cat");

	gdata_entry_set_summary (GDATA_ENTRY (file_entry), "Photo");

	/* Create an upload stream for the file. This is non-blocking. */
	upload_stream = gdata_picasaweb_service_upload_file (service, NULL, file_entry, g_file_info_get_display_name (file_info),
		                                             g_file_info_get_content_type (file_info), NULL, NULL);
	g_object_unref (file_info);
	g_object_unref (file_entry);
	
	/* Prepare a file stream for the file to be uploaded. This is a blocking operation. */
	file_stream = g_file_read (file_data, NULL, NULL);
	g_object_unref (file_data);

	/* Upload the file to the server. Note that this is a blocking operation. */
	error = NULL;
	g_output_stream_splice (G_OUTPUT_STREAM (upload_stream), G_INPUT_STREAM (file_stream),
		                G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE | G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET, NULL, &error);
	if (error != NULL)
	{
		/* Occuring unexpectedly. OUTPUT: "error: Authentication required. Authorization required."
		 * There is already a guard condition at Line #40 if the service is not authorized.
		 * The error message here does not make sense.*/
		printf("error : %s\n", error->message);
		return -1;
	}

	/* Parse the resulting updated entry. This is a non-blocking operation. */
	uploaded_file_entry = gdata_picasaweb_service_finish_file_upload (service, upload_stream, NULL);

	g_object_unref (file_stream);
	g_object_unref (upload_stream);
	g_object_unref (service);
	g_object_unref (authorizer);
	g_object_unref (uploaded_file_entry);

}