#include "psq4_gfx.h"
#include <string.h>
#include <esp_log.h>


static const char * PSQ4_GFX_TAG = "psq4-gfx";


esp_err_t psq4_gfx_init(
    psq4_gfx_canvas_t *canvas,
    psq4_gfx_dim_t *dim)
{
    canvas->dim = *dim;
    canvas->mutex = xSemaphoreCreateMutex();
    canvas->updates = xSemaphoreCreateBinary();
    if (!canvas->mutex || !canvas->updates) {
        ESP_LOGE(PSQ4_GFX_TAG, "Unable to allocate semaphores");
        return ESP_ERR_NO_MEM;
    }
    canvas->dirty_row_cursor = 0;
    canvas->row_size_bytes = dim->w * 2;
    canvas->dirty_row_count = dim->h;
    canvas->dirty_rows = (bool *) calloc(dim->h, sizeof(bool));
    if (!canvas->dirty_rows) {
        ESP_LOGE(PSQ4_GFX_TAG, "Unable to allocate dirty rows array");
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < dim->h; i++) canvas->dirty_rows[i] = true;
    canvas->data = (uint16_t *) calloc(dim->w * dim->h, sizeof(uint16_t));
    if (!canvas->data) {
        ESP_LOGE(PSQ4_GFX_TAG, "Unable to allocate canvas data buffer");
        return ESP_ERR_NO_MEM;
    }
    xSemaphoreGive(canvas->updates);
    xSemaphoreGive(canvas->mutex);
    return ESP_OK;
}


void psq4_gfx__flush(
    psq4_gfx_canvas_t *canvas,
    void *buffer,
    size_t max_len_bytes,
    psq4_gfx_bounds_t *bounds,
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


esp_err_t psq4_gfx_flush(
    psq4_gfx_canvas_t *canvas,
    void * buffer,
    size_t max_len_bytes,
    psq4_gfx_bounds_t *bounds,
    size_t * len_bytes)
{
    if (max_len_bytes < canvas->row_size_bytes) {
        ESP_LOGE(
            PSQ4_GFX_TAG,
            "psq4_gfx_flush(...) called with buffer too small to hold a single row"
        );
        return ESP_ERR_INVALID_SIZE;
    }
    if (xSemaphoreTake(canvas->updates, portMAX_DELAY) == pdTRUE) {
        if (xSemaphoreTake(canvas->mutex, portMAX_DELAY) == pdTRUE) {
            if (canvas->dirty_row_count > 0) {
                psq4_gfx__flush(canvas, buffer, max_len_bytes, bounds, len_bytes);
            }
            if (canvas->dirty_row_count > 0) {
                xSemaphoreGive(canvas->updates);
            }
            xSemaphoreGive(canvas->mutex);
        } else {
            ESP_LOGE(PSQ4_GFX_TAG, "Impossible timeout encountered");
            esp_restart();
        }
    } else {
        ESP_LOGE(PSQ4_GFX_TAG, "Impossible timeout encountered");
        esp_restart();
    }
    return ESP_OK;
}


static void psq4_gfx__dirty_row(
    psq4_gfx_canvas_t *canvas,
    uint8_t y)
{
    if (!canvas->dirty_rows[y]) {
        canvas->dirty_rows[y] = true;
        canvas->dirty_row_count++;
        xSemaphoreGive(canvas->updates);
    }
}


static void psq4_gfx__dirty_bounds(
    psq4_gfx_canvas_t *canvas,
    psq4_gfx_bounds_t *bounds)
{
    for (size_t y = bounds->y0; y <= bounds->y1; y++) {
        psq4_gfx__dirty_row(canvas, y);
    }
}


static bool psq4_gfx__invalid_coords(
    psq4_gfx_canvas_t *canvas,
    psq4_gfx_coords_t *coords)
{
    if (coords->x >= canvas->dim.w) return true;
    if (coords->y >= canvas->dim.h) return true;
    return false;
}


static bool psq4_gfx__invalid_bounds(
    psq4_gfx_canvas_t *canvas,
    psq4_gfx_bounds_t *bounds)
{
    if (bounds->x1 >= canvas->dim.w) return true;
    if (bounds->x0 >= canvas->dim.w) return true;
    if (bounds->x0 > bounds->x1) return true;
    if (bounds->y1 >= canvas->dim.h) return true;
    if (bounds->y0 >= canvas->dim.h) return true;
    if (bounds->y0 > bounds->y1) return true;
    return false;
}


esp_err_t psq4_gfx_constrain(
    psq4_gfx_canvas_t *canvas,
    psq4_gfx_bounds_t *bounds)
{
    if (bounds->x1 >= canvas->dim.w) {
        bounds->x1 = canvas->dim.w - 1;
    }
    if (bounds->y1 >= canvas->dim.h) {
        bounds->y1= canvas->dim.h - 1;
    }
    if (psq4_gfx__invalid_bounds(canvas, bounds)) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}


esp_err_t psq4_gfx_fill_px(
    psq4_gfx_canvas_t *canvas,
    uint16_t color,
    psq4_gfx_coords_t *coords)
{
    if (psq4_gfx__invalid_coords(canvas, coords)) {
        ESP_LOGE(
            PSQ4_GFX_TAG,
            "gfx_fill_px(...) called with invalid coords"
        );
        return ESP_ERR_INVALID_ARG;
    }
    size_t j;
    if (xSemaphoreTake(canvas->mutex, portMAX_DELAY) == pdTRUE) {
        j = (coords->y * canvas->dim.w) + coords->x;
        canvas->data[j] = color;
        psq4_gfx__dirty_row(canvas, coords->y);
        xSemaphoreGive(canvas->mutex);
        return ESP_OK;
    } else {
        ESP_LOGE(
            PSQ4_GFX_TAG,
            "gfx_fill_px failed to acquire gfx semaphore"
        );
        esp_restart();
    }
    return ESP_OK;
}


esp_err_t psq4_gfx_fill_rect(
  psq4_gfx_canvas_t *canvas,
  uint16_t color,
  psq4_gfx_bounds_t *bounds)
{
    if (psq4_gfx__invalid_bounds(canvas, bounds)) {
        ESP_LOGE(
            PSQ4_GFX_TAG,
            "psq4_gfx_fill_rect(...) called with invalid bounds"
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
        psq4_gfx__dirty_bounds(canvas, bounds);
        xSemaphoreGive(canvas->mutex);
    } else {
        ESP_LOGE(
            PSQ4_GFX_TAG,
            "psq4_gfx_fill_rect(...) failed to acquire gfx semaphore"
        );
        esp_restart();
    }
    return ESP_OK;
}


esp_err_t psq4_gfx_render_sprite(
    psq4_gfx_canvas_t *canvas,
    const psq4_gfx_sprite_t *sprite,
    const psq4_gfx_coords_t *origin,
    psq4_gfx_bounds_t *bounds)
{
    esp_err_t ret;
    bounds->x0 = origin->x;
    bounds->y0 = origin->y;
    bounds->x1 = bounds->x0 + sprite->dim.w - 1;
    bounds->y1 = bounds->y0 + sprite->dim.h - 1;
    ret = psq4_gfx_constrain(canvas, bounds);
    if (ret != ESP_OK) {
        ESP_LOGE(
            PSQ4_GFX_TAG,
            "Invalid origin provided to psq4_gfx_render_sprite(...)"
        );
        return ret;
    }
    if (xSemaphoreTake(canvas->mutex, portMAX_DELAY) == pdTRUE) {
        size_t src, dst;
        uint8_t src_row = 0;
        for (uint8_t y = bounds->y0; y <= bounds->y1; y++) {
            src = src_row * sprite->dim.w;
            for (uint8_t x = bounds->x0; x <= bounds->x1; x++) {
                dst = (y * canvas->dim.w) + x;
                canvas->data[dst] = sprite->data[src];
                src++;
            }
            src_row++;
        }
        psq4_gfx__dirty_bounds(canvas, bounds);
        xSemaphoreGive(canvas->mutex);
    } else {
        ESP_LOGE(
            PSQ4_GFX_TAG,
            "psq4_gfx_render_sprite failed to acquire gfx semaphore"
        );
        esp_restart();
    }
    return ESP_OK;
}
