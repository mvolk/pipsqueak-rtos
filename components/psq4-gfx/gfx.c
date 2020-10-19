#include "gfx.h"
#include "sprites.h"
#include <string.h>
#include <esp_log.h>

#include <esp_heap_trace.h>

static const char * GFX_TAG = "gfx";

#define DIM_UNDEFINED 255


esp_err_t gfx_init(gfx_canvas_t *canvas, gfx_dim_t *dim) {
    canvas->dim = *dim;
    canvas->mutex = xSemaphoreCreateMutex();
    canvas->updates = xSemaphoreCreateBinary();
    if (!canvas->mutex || !canvas->updates) {
        ESP_LOGE(GFX_TAG, "Unable to allocate semaphores");
        return ESP_ERR_NO_MEM;
    }
    canvas->dirty_row_cursor = 0;
    canvas->row_size_bytes = dim->w * 2;
    canvas->dirty_row_count = dim->h;
    canvas->dirty_rows = (bool *) calloc(dim->h, sizeof(bool));
    if (!canvas->dirty_rows) {
        ESP_LOGE(GFX_TAG, "Unable to allocate dirty rows array");
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < dim->h; i++) canvas->dirty_rows[i] = true;
    canvas->data = (uint16_t *) calloc(dim->w * dim->h, sizeof(uint16_t));
    if (!canvas->data) {
        ESP_LOGE(GFX_TAG, "Unable to allocate canvas data buffer");
        return ESP_ERR_NO_MEM;
    }
    xSemaphoreGive(canvas->updates);
    xSemaphoreGive(canvas->mutex);
    return ESP_OK;
}


void gfx__flush(
    gfx_canvas_t *canvas,
    void *buffer,
    size_t max_len_bytes,
    gfx_bounds_t *bounds,
    size_t *len_bytes)
{
    if (canvas->dirty_row_count == 0) return;
    size_t capacity = max_len_bytes / canvas->row_size_bytes;
    bounds->x0 = 0;
    bounds->x1 = canvas->dim.w - 1;
    // Advance to the first dirty row (we know there is one)
    while (!canvas->dirty_rows[canvas->dirty_row_cursor]) {
        canvas->dirty_row_cursor = (canvas->dirty_row_cursor + 1) % canvas->dim.h;
    }
    bounds->y0 = canvas->dirty_row_cursor;
    size_t src_cursor;
    size_t dst_cursor = 0;
    *len_bytes = 0;
    // Terminate when we reach a clean row or fill the capacity of the buffer
    while(canvas->dirty_rows[canvas->dirty_row_cursor] && capacity > 0) {
        src_cursor = canvas->dirty_row_cursor * canvas->row_size_bytes;
        memcpy(&(((uint8_t *)buffer)[dst_cursor]), &(((uint8_t *)(canvas->data))[src_cursor]), canvas->row_size_bytes);
        *len_bytes += canvas->row_size_bytes;
        canvas->dirty_rows[canvas->dirty_row_cursor] = false;
        bounds->y1 = canvas->dirty_row_cursor;
        canvas->dirty_row_count = canvas->dirty_row_count - 1;
        capacity--;
        dst_cursor += canvas->row_size_bytes;
        canvas->dirty_row_cursor = canvas->dirty_row_cursor + 1;
        if (canvas->dirty_row_cursor == canvas->dim.h) {
            canvas->dirty_row_cursor = 0;
            break;
        }
    }
}


esp_err_t gfx_flush(
    gfx_canvas_t *canvas,
    void * buffer,
    size_t max_len_bytes,
    gfx_bounds_t *bounds,
    size_t * len_bytes)
{
    if (max_len_bytes < canvas->row_size_bytes) {
        ESP_LOGE(GFX_TAG, "gfx_clean called with buffer too small to hold a single row");
        return ESP_ERR_INVALID_SIZE;
    }
    if (xSemaphoreTake(canvas->updates, portMAX_DELAY) == pdTRUE) {
        if (xSemaphoreTake(canvas->mutex, portMAX_DELAY) == pdTRUE) {
            if (canvas->dirty_row_count > 0) {
                gfx__flush(canvas, buffer, max_len_bytes, bounds, len_bytes);
            }
            if (canvas->dirty_row_count > 0) {
                xSemaphoreGive(canvas->updates);
            }
            xSemaphoreGive(canvas->mutex);
        } else {
            ESP_LOGE(GFX_TAG, "Impossible timeout encountered");
            esp_restart();
        }
    } else {
        ESP_LOGE(GFX_TAG, "Impossible timeout encountered");
        esp_restart();
    }
    return ESP_OK;
}


static void gfx__dirty_row(
    gfx_canvas_t *canvas,
    uint8_t y)
{
    if (!canvas->dirty_rows[y]) {
        canvas->dirty_rows[y] = true;
        canvas->dirty_row_count++;
        xSemaphoreGive(canvas->updates);
    }
}


static void gfx__dirty_bounds(
    gfx_canvas_t *canvas,
    gfx_bounds_t *bounds)
{
    for (size_t y = bounds->y0; y <= bounds->y1; y++) {
        gfx__dirty_row(canvas, y);
    }
}


static bool gfx__invalid_coords(
    gfx_canvas_t *canvas,
    gfx_coords_t *coords)
{
    if (coords->x >= canvas->dim.w) return true;
    if (coords->y >= canvas->dim.h) return true;
    return false;
}


static bool gfx__invalid_bounds(
    gfx_canvas_t *canvas,
    gfx_bounds_t *bounds)
{
    if (bounds->x1 >= canvas->dim.w) return true;
    if (bounds->x0 >= canvas->dim.w) return true;
    if (bounds->x0 > bounds->x1) return true;
    if (bounds->y1 >= canvas->dim.h) return true;
    if (bounds->y0 >= canvas->dim.h) return true;
    if (bounds->y0 > bounds->y1) return true;
    return false;
}


esp_err_t gfx_constrain(gfx_canvas_t *canvas, gfx_bounds_t *bounds) {
    if (bounds->x1 >= canvas->dim.w) {
        bounds->x1 = canvas->dim.w - 1;
    }
    if (bounds->y1 >= canvas->dim.h) {
        bounds->y1= canvas->dim.h - 1;
    }
    if (gfx__invalid_bounds(canvas, bounds)) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}


esp_err_t gfx_fill_px(
    gfx_canvas_t *canvas,
    uint16_t color,
    gfx_coords_t *coords)
{
    if (gfx__invalid_coords(canvas, coords)) {
        ESP_LOGE(
            GFX_TAG,
            "gfx_fill_px(...) called with invalid coords"
        );
        return ESP_ERR_INVALID_ARG;
    }
    size_t j;
    if (xSemaphoreTake(canvas->mutex, portMAX_DELAY) == pdTRUE) {
        j = (coords->y * canvas->dim.w) + coords->x;
        canvas->data[j] = color;
        gfx__dirty_row(canvas, coords->y);
        xSemaphoreGive(canvas->mutex);
        return ESP_OK;
    } else {
        ESP_LOGE(GFX_TAG, "gfx_fill_px failed to acquire gfx semaphore");
        esp_restart();
    }
    return ESP_OK;
}


esp_err_t gfx_fill_rect(
  gfx_canvas_t *canvas,
  uint16_t color,
  gfx_bounds_t *bounds)
{
    if (gfx__invalid_bounds(canvas, bounds)) {
        ESP_LOGE(
            GFX_TAG,
            "gfx_fill_rect(...) called with invalid bounds"
        );
        return ESP_ERR_INVALID_ARG;
    }
    if (xSemaphoreTake(canvas->mutex, portMAX_DELAY) == pdTRUE) {
        size_t j;
        for (uint8_t y = bounds->y0; y <= bounds->y1; y++) {
            for (uint8_t x = bounds->x0; x <= bounds->x1; x++) {
                j = (y * canvas->dim.w) + x;
                canvas->data[j] = color;
            }
        }
        gfx__dirty_bounds(canvas, bounds);
        xSemaphoreGive(canvas->mutex);
    } else {
        ESP_LOGE(GFX_TAG, "gfx_fill_px failed to acquire gfx semaphore");
        esp_restart();
    }
    return ESP_OK;
}


static esp_err_t gfx__render_sprite(
    gfx_canvas_t *canvas,
    const uint8_t *sprite,
    const gfx_dim_t *sprite_dim,
    const gfx_coords_t *origin,
    gfx_bounds_t *bounds)
{
    esp_err_t ret;
    bounds->x0 = origin->x;
    bounds->y0 = origin->y;
    bounds->x1 = bounds->x0 + sprite_dim->w - 1;
    bounds->y1 = bounds->y0 + sprite_dim->h - 1;
    ret = gfx_constrain(canvas, bounds);
    if (ret != ESP_OK) {
        ESP_LOGE(
            GFX_TAG,
            "Invalid origin provided to gfx_render_sprite(...)"
        );
        return ret;
    }
    if (xSemaphoreTake(canvas->mutex, portMAX_DELAY) == pdTRUE) {
        size_t src, dst;
        uint8_t src_row = 0;
        for (uint8_t y = bounds->y0; y <= bounds->y1; y++) {
            src = src_row * sprite_dim->w;
            for (uint8_t x = bounds->x0; x <= bounds->x1; x++) {
                dst = (y * canvas->dim.w) + x;
                canvas->data[dst] = ((uint16_t *) sprite)[src];
                src++;
            }
            src_row++;
        }
        gfx__dirty_bounds(canvas, bounds);
        xSemaphoreGive(canvas->mutex);
        return ESP_OK;
    } else {
        ESP_LOGE(GFX_TAG, "gfx_fill_px failed to acquire gfx semaphore");
        esp_restart();
    }
}


esp_err_t gfx_render_sprite(
    gfx_canvas_t *canvas,
    gfx_sprite_t sprite,
    const gfx_coords_t *origin,
    gfx_bounds_t *bounds)
{
    switch (sprite) {
        case gfx_sprite_wifi_ok:
            return gfx__render_sprite(
                canvas,
                gfx_sprite_wifi_ok_data,
                &gfx_sprite_wifi_ok_dim,
                origin,
                bounds
            );
        case gfx_sprite_wifi_fail:
            return gfx__render_sprite(
                canvas,
                gfx_sprite_wifi_fail_data,
                &gfx_sprite_wifi_fail_dim,
                origin,
                bounds
            );
        default:
            ESP_LOGE(
                GFX_TAG,
                "Unsupported sprite: %d",
                sprite
            );
            return ESP_ERR_INVALID_ARG;
    }
}
