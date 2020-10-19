#ifndef PSQ4_GFX_H
#define PSQ4_GFX_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>


// This is a naive implementation of a simple graphics
// library for prototyping purposes only.


/**
 * @brief 2D dimensions - width and height
 *
 * Dimensions limited to 256 pixels in any one
 * direction.
 */
typedef struct {
    uint8_t w;
    uint8_t h;
} psq4_gfx_dim_t;


/**
 * @brief 2D coordinates - x and y
 *
 * Coordinate values must not exceed 255.
 */
typedef struct {
    uint8_t x;
    uint8_t y;
} psq4_gfx_coords_t;


/**
 * @brief 2D rectangle location and size
 *
 * Coordinate values must not exceed 255.
 */
typedef struct {
    uint8_t x0;
    uint8_t y0;
    uint8_t x1;
    uint8_t y1;
} psq4_gfx_bounds_t;


/**
 * @brief A small image, icon, character, etc.
 */
typedef struct {
    uint16_t *data;
    psq4_gfx_dim_t dim;
} psq4_gfx_sprite_t;


/**
 * @brief 2D in-memory graphics canvas
 *
 * This buffer consumes large quantities of DRAM!
 *
 * 160x80  = ~26kB
 * 128x128 = ~33kB
 * 128x160 = ~41kB
 * 240x135 = ~65kB
 * 240x240 = ~116kB
 *
 * Canvas size is limited to 256 pixels in any
 * dimension.
 */
typedef struct {
    /** @brief canvas data buffer */
    uint16_t *data;
    /** @brief canvas dimensions */
    psq4_gfx_dim_t dim;
    /** @brief mutex for read/write ops */
    SemaphoreHandle_t mutex;
    /**
     * @brief semaphore for signalling the existence
     * of updates that need to be flushed to the
     * display.
     */
    SemaphoreHandle_t updates;
    /**
     * @brief The coordinate of the next row to
     * consider for flushing to the display.
     */
    size_t dirty_row_cursor;
    /** @brief Number of bytes per row */
    size_t row_size_bytes;
    /**
     * @brief Number of rows that contain updates
     */
    size_t dirty_row_count;
    /**
     * @brief Indicates which rows contain updates
     *
     * Array of booleans indexed by row coordinate,
     * each true only if the row with that coordinate
     * contains updates that need to be flushed to
     * the display.
     */
    bool *dirty_rows;
} psq4_gfx_canvas_t;


/**
 * @brief Initialize the canvas.
 *
 * Given an empty canvas struct, this function
 * allocates memory and fully populates the struct.
 *
 * @param canvas the canvas to initialize
 * @param dim the desired canvas dimensions
 * @return ESP_OK if everything went well, otherwise
 *         an error indicating what went wrong.
 */
esp_err_t psq4_gfx_init(
    psq4_gfx_canvas_t *canvas,
    psq4_gfx_dim_t *dim
);


/**
 * @brief Flushes updates to a buffer
 *
 * Copies as many contiguous rows with updates as
 * possible to the provided buffer.
 *
 * @param canvas The canvas to flush
 * @param buffer The buffer into which the canvas
 *        is to be flushed
 * @param max_len_bytes The size of the buffer
 * @param bounds The bounds of the data written
 *        to the buffer, set by this function
 * @param len_bytes The number of bytes written
 *        to the buffer, set by this function
 * @return ESP_OK if everything went well, otherwise
 *         an error indicating what went wrong.
 */
esp_err_t psq4_gfx_flush(
    psq4_gfx_canvas_t *canvas,
    void * buffer,
    size_t max_len_bytes,
    psq4_gfx_bounds_t *bounds,
    size_t * len_bytes
);


/**
 * @brief Constrain bounds to a canvas
 *
 * Fails if x0 or y0 are beyond the edge
 * of the canvas.
 *
 * Sets x1 and/or y1 to be the maximum
 * coordinate dimension within the canvas
 * if either is beyond the dimension limits
 * of the canvas.
 *
 * Does nothing if x0, y0, x1 and y1 are
 * all within the bounds of the canvas.
 *
 * @param canvas The constraint
 * @param bounds To be constrained
 * @return ESP_OK if everything went well, otherwise
 *         an error indicating what went wrong.
 */
esp_err_t psq4_gfx_constrain(
    psq4_gfx_canvas_t *canvas,
    psq4_gfx_bounds_t *bounds
);


/**
 * @brief Set the color of a single pixel
 *
 * While this can also be done with gfx_fill_rect(..),
 * this method is slightly more efficient.
 *
 * @param canvas The canvas to paint on
 * @param color The big-endian RGB/565 color to paint
 * @param coords The pixel to paint
 * @return ESP_OK if everything went well, otherwise
 *         an error indicating what went wrong.
 */
esp_err_t psq4_gfx_fill_px(
    psq4_gfx_canvas_t *canvas,
    uint16_t color,
    psq4_gfx_coords_t *coords
);


/**
 * @brief Fills a rectangle with a single color
 *
 * @param canvas The canvas to paint on
 * @param color The big-endian RGB/565 color to paint
 * @param bounds The rectangle to fill
 * @return ESP_OK if everything went well, otherwise
 *         an error indicating what went wrong.
 */
esp_err_t psq4_gfx_fill_rect(
    psq4_gfx_canvas_t *canvas,
    uint16_t color,
    psq4_gfx_bounds_t *bounds
);


/**
 * @brief Renders a sprite
 *
 * Sprites are enumerated for ease of identification.
 *
 * So long as the origin is on the canvas, any portion
 * of the sprite that extends beyond the canvas is
 * simply truncated.
 *
 * @param canvas The canvas to paint on
 * @param sprite The sprite's image data
 * @param sprite_dim The sprite's dimensions
 * @param origin The coordinates on the canvas where
 *        the lower left-most pixel of the sprite will
 *        be rendered
 * @param bounds The bounds of the sprite as rendered,
 *        set by this function
 * @return ESP_OK if everything went well, otherwise
 *         an error indicating what went wrong.
 */
esp_err_t psq4_gfx_render_sprite(
    psq4_gfx_canvas_t *canvas,
    const psq4_gfx_sprite_t *sprite,
    const psq4_gfx_coords_t *origin,
    psq4_gfx_bounds_t *bounds
);


#endif // PSQ4_GFX_H
