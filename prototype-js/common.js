//-- State

// https://stackoverflow.com/a/37164538/29393722
function isObject(item) {
  return item && typeof item === "object" && !Array.isArray(item);
}

function mergeDeep(target, source) {
  let output = Object.assign({}, target);
  if (isObject(target) && isObject(source)) {
    Object.keys(source).forEach((key) => {
      if (isObject(source[key])) {
        if (!(key in target)) Object.assign(output, { [key]: source[key] });
        else output[key] = mergeDeep(target[key], source[key]);
      } else {
        Object.assign(output, { [key]: source[key] });
      }
    });
  }
  return output;
}

/**
 * @template T
 * @param {T} initial
 * @returns {T & { set(nextState: Partial<T>): T }}
 */
const createState = (initial) => {
  return {
    ...initial,

    set(nextState) {
      const merged = mergeDeep(this, nextState);
      Object.assign(this, merged);
      return this;
    },

    replace(key, value) {
      this[key] = value;
      return this;
    },
  };
};

//-- Values

const clamp = (min, max, v) => Math.max(min, Math.min(max, v));

//-- Colors

const hexToRgba = (hex, alpha = 1) => {
  const h = hex.replace("#", "");
  return [
    parseInt(h.substr(0, 2), 16) / 255,
    parseInt(h.substr(2, 2), 16) / 255,
    parseInt(h.substr(4, 2), 16) / 255,
    alpha,
  ];
};

const lerpColor = (c1, c2, t) => {
  return [
    c1[0] + (c2[0] - c1[0]) * t,
    c1[1] + (c2[1] - c1[1]) * t,
    c1[2] + (c2[2] - c1[2]) * t,
    c1[3] + (c2[3] - c1[3]) * t,
  ];
};

//-- Drawing

const clearBackground = (mgraphics, color, rounded = 0) => {
  const [width, height] = mgraphics.size;

  mgraphics.set_source_rgba(color);
  mgraphics.rectangle_rounded(0, 0, width, height, rounded, rounded);
  mgraphics.fill();
};

const drawText = (
  mgraphics,
  boxX,
  boxY,
  boxWidth,
  boxHeight,
  text,
  options,
) => {
  const defaultOpts = {
    face: "Arial",
    weight: "bold",
    size: 9,
    color: hexToRgba("#eeeeee"),
    alignX: "center",
    alignY: "center",
    paddingX: 0,
    paddingY: 0,
  };
  const opts = { ...defaultOpts, ...options };

  mgraphics.select_font_face(opts.face, opts.weight);
  mgraphics.set_font_size(opts.size);

  const [textWidth] = mgraphics.text_measure(text);
  const [ascent, descent] = mgraphics.font_extents();

  let textX;
  switch (opts.alignX) {
    case "left":
      textX = boxX + opts.paddingX;
      break;
    case "center":
      textX = (boxWidth - textWidth) / 2 + boxX;
      break;
    case "right":
      textX = boxWidth - textWidth + boxX - opts.paddingX;
      break;
    default:
      textX = boxX;
  }

  let textY;
  switch (opts.alignY) {
    case "top":
      textY = boxY + ascent + opts.paddingY;
      break;
    case "center":
      textY = boxHeight / 2 + ascent / 2 - descent / 2 + boxY;
      break;
    case "bottom":
      textY = boxY + boxHeight - descent - opts.paddingY;
      break;
    default:
      textY = boxY + ascent;
  }

  mgraphics.set_source_rgba(opts.color);
  mgraphics.move_to(textX, textY);
  mgraphics.text_path(text);
  mgraphics.fill();
};

module.exports = {
  createState,
  clamp,
  hexToRgba,
  lerpColor,
  clearBackground,
  drawText,
};
