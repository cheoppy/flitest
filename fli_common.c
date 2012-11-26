#include "fli_common.h"

/* domains for the FLIList command  */
flidomain_t FLI_USB_CAMERA_DOMAIN = FLIDOMAIN_USB | FLIDEVICE_CAMERA;
flidomain_t FLI_USB_FILTER_DOMAIN = FLIDOMAIN_USB | FLIDEVICE_FILTERWHEEL;

/* global status variable for the fli devices */
fli_status fli_m = {
  .num_filters = 0,
  .num_cameras = 0,
  .cameras = NULL,
  .filters = NULL,
  .current_filter = -1,
  .active_camera = FLI_INVALID_DEVICE,
  .active_filter = FLI_INVALID_DEVICE,
  .set_1mhz_speed = 0
};
fli_status * fli = &fli_m;

/**
 * Translates the error codes of libfli to legible error messages
 *
 * @param error Error code provided by libfli
 * @param source Name of the FLI call that throw the error
 * @return
 */
int fli_check_error(int error, char * source) {
  if (error) {
    fprintf(stderr, "\nFLI ERROR[%s]: %s\n", source, strerror((int) - 1 * error));
    return 1;
  }
  return 0;
}

/**
 * Opens and lock the first (having an index of 0) camera
 *
 * @param ccdsh_fli_dev
 * @return
 */
int fli_open_and_lock_first_camera(flidev_t * fli_dev) {
  return fli_open_and_lock_camera(0, fli_dev);
}

/**
 * Opens and lock the first (having an index of 0) filter
 *
 * @param ccdsh_fli_dev
 * @return
 */
int fli_open_and_lock_first_filter(flidev_t * fli_dev) {
  return fli_open_and_lock_filter(0, fli_dev);
}

/**
 * Opens and locks the camera of index i
 *
 * @param i index of the camera to be opened and locked
 * @param ccdsh_fli_dev
 * @return
 */
int fli_open_and_lock_camera(int i, flidev_t * fli_dev) {
  if (fli->num_cameras == 0 || fli->num_cameras < i + 1) {
    return -1;
  } else {
    return fli_open_and_lock_device(i, fli_dev, fli->cameras, FLI_USB_CAMERA_DOMAIN);
  }
}

/**
 * Opens and locks the filter of index i
 *
 * @param i index of the filter to be opened and locked
 * @param fli_dev
 * @return
 */
int fli_open_and_lock_filter(int i, flidev_t * fli_dev) {
  if (fli->num_filters == 0 || fli->num_filters < i + 1) {
    return -1;
  } else {
    return fli_open_and_lock_device(i, fli_dev, fli->filters, FLI_USB_FILTER_DOMAIN);
  }
}

/**
 * Opens and lock the device of index i
 *
 * @param i index of the device to be opened and locked
 * @param fli_dev
 * @param device_list
 * @param domain
 * @return
 */
int fli_open_and_lock_device(int i, flidev_t *fli_dev, char ** device_list, flidomain_t domain) {
  int err, k;

  for (k = 0; device_list[k] != NULL; k++);
  if (i > k) {
    fprintf(stderr, "there is no device with index %d\n", k);
    return -1;
  }

  /*    Open the device[k]    */
  err = FLIOpen(fli_dev, device_list[i], domain);
  if (fli_check_error(err, "FLIOpen")) return -1;
  /*    Lock the device[k]    */
  err = FLILockDevice(*fli_dev);
  if (fli_check_error(err, "FLILockDevice")) return -1;
  return 0;
}

/**
 * Unlocks and closes the fli_dev device
 *
 * @param ccdsh_fli_dev
 * @return
 */
int fli_unlock_and_close_device(flidev_t * fli_dev) {
  int err;

  /* Unlock the device */
  err = FLIUnlockDevice(*fli_dev);
  if (fli_check_error(err, "FLIUnlockDevice")) return -1;

  /* Close the device */
  err = FLIClose(*fli_dev);
  if (fli_check_error(err, "FLIClose")) return -1;

  return 0;
}

/**
 * Scans for FLI cameras and filters; and opens the filter if there is exactly one filter.
 *
 * The found devices are put in the fli->cameras and fli->filters arrays,
 * accordingly and their number is set in fli->num_cameras and fli->num_filters.
 *
 * @return
 */
int fli_scan() {
  int ret;
  ret = fli_scan_domain("camera", FLI_USB_CAMERA_DOMAIN, &fli->cameras, &fli->num_cameras);
  if (ret) return ret;

  ret = fli_scan_domain("filter", FLI_USB_FILTER_DOMAIN, &fli->filters, &fli->num_filters);
  if (ret) return ret;

  return 0;
}

/**
 * Scans for one type of FLI device
 *
 * @param device_type: "camera" or "filter"
 * @return 0 on success, non-zero on error
 */
int fli_scan_domain(char * device_type, flidomain_t domain, char *** device_list, int *device_count) {
  char ** fli_list;
  int k, err;

  /* get list of FLI cameras or filters according to $domain */
  err = FLIList(domain, &fli_list);
  if (fli_check_error(err, "FLIList")) return -1;

  /* count the number of found devices*/
  for (k = 0; fli_list[k] != NULL; k++) {
    ;
  }
  *device_count = k;


  /* store the number of devices in the according struct */
  *device_list = (char**) malloc((*device_count) * sizeof (char*));
  for (k = 0; fli_list[k] != NULL; k++) {
    char semicolon = ';';
    char *quotPtr = strchr(fli_list[k], semicolon);
    if (quotPtr == NULL) {
      fprintf(stderr, "cannot parse fli_list[k]: %s\n", fli_list[k]);
      return -1;
    }
    int position = quotPtr - fli_list[k];
    (*device_list)[k] = (char*) malloc((position + 1) * sizeof (char));
    memcpy((*device_list)[k], fli_list[k], position);
    (*device_list)[k][position] = '\0';
  }

  /* free the FLI device list */
  err = FLIFreeList(fli_list);
  if (fli_check_error(err, "FLIFreeList")) return -1;

  /* if we got so far, then there were no errors, and we can return 0 as success */
  return 0;
}
