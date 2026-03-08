/*-------------------------------- Presetter ---------------------------------*/

const {
  createState,
  clearBackground,
  drawText,
  hexToRgba,
} = require("./common.js");

// *----------------*
// | Initialization |
// *----------------*

autowatch = 1;
inlets = 2;
outlets = 2;

mgraphics.init();

// *-------*
// | State |
// *-------*

const state = createState({
  // General
  focused: false,

  // Presets
  slots: {},
  presetName: null,

  // Status
  status: "Ready",
  hoveredStatus: null,
  preferStatus: false,

  // Hovering
  hoveredCell: null,
  selectedCell: null,
  lastX: 0,

  // Name editing
  writeButtonDown: false,
  editingName: false,

  // Confirming
  confirmStatus: null,
  confirmCell: null,
  confirmStore: false,
  confirmDelete: false,
  confirmOkDown: false,
  confirmCancelDown: false,

  // Refresh
  refreshed: false,
});

// *---------*
// | Globals |
// *---------*

const GRID_OFFSET_X = 5;
const GRID_OFFSET_Y = 23;
const CELL_SIZE = 7;
const CELL_PADDING = 3.5;
const CELL_TOTAL_SIZE = CELL_SIZE + CELL_PADDING;

// *--------*
// | Colors |
// *--------*

const colors = {
  // Global
  bg: hexToRgba("#1e1e1e"),
  // Write Button
  writeButtonOn: max.getcolor("live_arranger_grid_tiles"),
  writeButtonOff: hexToRgba("#444444"),
  writeButtonClickText: max.getcolor("live_contrast_frame"),
  // Cells
  selectedCell: max.getcolor("live_display_handle_two"),
  hoveredCell: hexToRgba("#999999"),
  storedCell: hexToRgba("#666666"),
  defaultCell: hexToRgba("#333333"),
  // Preset Name
  presetNameEditingBg: max.getcolor("live_arranger_grid_tiles"),
  presetNameTextUnselected: hexToRgba("#555555"),
  presetNameTextEditing: max.getcolor("live_contrast_frame"),
  presetNameTextDefault: max.getcolor("live_arranger_grid_tiles"),
  // Status
  statusText: hexToRgba("#777777"),
  // Refresh
  refreshOn: patcher.color,
  refreshOff: hexToRgba("#333333"),
};

// *------------------*
// | Message Handlers |
// *------------------*

let refreshTask = null;

function bang() {
  if (inlet === 0) {
    state.set({ refreshed: true });
    outlet(0, "getslotnamelist");

    if (refreshTask) {
      refreshTask.cancel();
    }

    refreshTask = new Task(function () {
      state.set({ refreshed: false });
      mgraphics.redraw();
    });
    refreshTask.schedule(300);
  }
}

function anything() {
  if (inlet === 1 && messagename === "slotname") {
    const [index, name] = arrayfromargs(arguments);

    if (index === 0) {
      state.replace("slots", {});
    }

    if (index === "done" || name === "(undefined)") return;

    state.set({ slots: { [index]: { index, name } } });
    mgraphics.redraw();
  }
}

// *------------------*
// | Common Functions |
// *------------------*

const getCellByXY = (x, y) => {
  const relX = x - GRID_OFFSET_X - CELL_PADDING / 2;
  const relY = y - GRID_OFFSET_Y - CELL_PADDING / 2;
  const cellX = Math.floor(relX / CELL_TOTAL_SIZE);
  const cellY = Math.floor(relY / CELL_TOTAL_SIZE);

  return [cellX, cellY];
};

// *---------------*
// | Object Bounds |
// *---------------*

// -- Grid

const gridDimensions = () => {
  const [width, height] = mgraphics.size;

  const gridWidth = width - CELL_PADDING - GRID_OFFSET_X;
  const gridHeight = height - CELL_PADDING - GRID_OFFSET_Y - 20;

  const rows = Math.floor(gridHeight / CELL_TOTAL_SIZE);
  const columns = Math.floor(gridWidth / CELL_TOTAL_SIZE);

  return [rows, columns];
};

const gridBounds = () => {
  const [rows, columns] = gridDimensions();
  const width = columns * CELL_TOTAL_SIZE + CELL_PADDING / 2;
  const height = rows * CELL_TOTAL_SIZE + CELL_PADDING / 2;

  return [GRID_OFFSET_X, GRID_OFFSET_Y, width, height];
};

const inGridBounds = (x, y) => {
  const [xPos, yPos, width, height] = gridBounds();

  const inXBounds = x >= xPos + CELL_PADDING && x <= width + xPos;
  const inYBounds = y >= yPos + CELL_PADDING && y <= height + yPos;

  return inXBounds && inYBounds;
};

// -- Write Button

const writeButtonBounds = () => {
  const [canvasWidth] = mgraphics.size;

  const width = 68;
  const height = 12;
  const topOffset = 8;
  const rightOffset = 8;

  const x = canvasWidth - width - rightOffset;
  const y = topOffset;

  return [x, y, width, height];
};

const inWriteButtonBounds = (x, y) => {
  const [xPos, yPos, width, height] = writeButtonBounds();

  const inXBounds = x >= xPos && x <= width + xPos;
  const inYBounds = y >= yPos && y <= height + yPos;

  return inXBounds && inYBounds;
};

// -- Confirm OK Button

const confirmOkButtonBounds = () => {
  const [, canvasHeight] = mgraphics.size;

  const width = 30;
  const height = 12;
  const bottomOffset = 7;
  const leftOffset = 90;

  const x = leftOffset;
  const y = canvasHeight - bottomOffset - height;

  return [x, y, width, height];
};

const inConfirmOkButtonBounds = (x, y) => {
  const [xPos, yPos, width, height] = confirmOkButtonBounds();

  const inXBounds = x >= xPos && x <= width + xPos;
  const inYBounds = y >= yPos && y <= height + yPos;

  return inXBounds && inYBounds;
};

// -- Confirm Cancel Button

const confirmCancelButtonBounds = () => {
  const [, canvasHeight] = mgraphics.size;

  const width = 50;
  const height = 12;
  const bottomOffset = 7;
  const leftOffset = 130;

  const x = leftOffset;
  const y = canvasHeight - bottomOffset - height;

  return [x, y, width, height];
};

const inConfirmCancelButtonBounds = (x, y) => {
  const [xPos, yPos, width, height] = confirmCancelButtonBounds();

  const inXBounds = x >= xPos && x <= width + xPos;
  const inYBounds = y >= yPos && y <= height + yPos;

  return inXBounds && inYBounds;
};

// -- Preset Name

const presetNameTextBounds = () => {
  const [canvasWidth] = mgraphics.size;
  const [, , buttonWidth] = writeButtonBounds();

  const x = 8;
  const y = 8;
  const width = canvasWidth - buttonWidth - 10;
  const height = 12;

  return [x, y, width, height];
};

const inPresetNameTextBounds = (x, y) => {
  const [xPos, yPos, width, height] = presetNameTextBounds();

  const inXBounds = x >= xPos && x <= width + xPos;
  const inYBounds = y >= yPos && y <= height + yPos;

  return inXBounds && inYBounds;
};

// *----------------*
// | Pointer events |
// *----------------*

// -- Focus / Blur

function onfocus() {
  state.set({ focused: true });
  mgraphics.redraw();
}

function onblur() {
  state.set({ focused: false, editingName: false });
  mgraphics.redraw();
}

// -- Hover In / Out

function onidle(x, y) {
  if (inGridBounds(x, y)) {
    const { slots, confirmStore, confirmDelete, lastX } = state;
    const [cellX, cellY] = getCellByXY(x, y);
    const [, columns] = gridDimensions();

    const hoveredCell = cellX + columns * cellY + 1;
    const slot = slots[hoveredCell];
    const slotName = slot && slot.name ? `: ${slot.name}` : " <empty>";
    const hoveredStatus = `Preset ${hoveredCell}` + slotName;
    const confirming = confirmStore || confirmDelete;
    const preferStatus = confirming || (state.preferStatus && lastX === x);

    state.set({ hoveredCell, hoveredStatus, preferStatus });
  } else {
    state.set({ hoveredCell: null, hoveredStatus: null });
  }

  state.set({ lastX: x });
  mgraphics.redraw();
}

function onidleout() {
  state.set({ hoveredCell: null });
  mgraphics.redraw();
}

// -- Click

// Cell Selection

const handleCellClick = (x, y, { recall, store, remove }) => {
  const [cellX, cellY] = getCellByXY(x, y);
  const [, columns] = gridDimensions();
  const selectedCell = cellX + columns * cellY + 1;

  if (recall && state.slots[selectedCell]) {
    const status = `Selected Preset ${selectedCell}`;

    state.set({
      selectedCell,
      presetName: state.slots[selectedCell].name,
      status,
      hoveredStatus: status,
      preferStatus: true,
      confirmStore: false,
      confirmDelete: false,
    });

    outlet(0, "recall", selectedCell);
  }

  if (store) {
    const confirmStatus = `Store Preset ${selectedCell}?`;
    state.set({
      confirmStatus,
      preferStatus: true,
      confirmStore: true,
      confirmDelete: false,
      confirmCell: selectedCell,
    });
  }

  if (remove) {
    const confirmStatus = `Delete Preset ${selectedCell}?`;
    state.set({
      confirmStatus,
      preferStatus: true,
      confirmStore: false,
      confirmDelete: true,
      confirmCell: selectedCell,
    });
  }
};

// Preset Storage Callbacks

const storePreset = () => {
  const { confirmCell } = state;
  const status = `Stored Preset ${confirmCell}`;

  state.set({ status, preferStatus: true, confirmCell: null });

  outlet(0, "store", confirmCell);
  outlet(0, "getslotnamelist");
  outlet(1, "bang");
};

const deletePreset = () => {
  const { confirmCell } = state;

  if (confirmCell === state.selectedCell) {
    state.set({ selectedCell: null });
  }

  const status = `Removed Preset ${state.confirmCell}`;

  state.set({ status, preferStatus: true, confirmCell: null });

  outlet(0, "delete", confirmCell);
  outlet(0, "getslotnamelist");
  outlet(1, "bang");
};

const renamePreset = (buttonDown) => {
  const status = `Renamed Preset ${state.selectedCell}`;

  state.set({
    writeButtonDown: buttonDown,
    editingName: false,
    status,
    hoveredStatus: status,
  });

  outlet(0, "slotname", state.selectedCell, state.presetName);
  outlet(0, "store", state.selectedCell);
  outlet(0, "getslotnamelist");
  outlet(1, "bang");
};

// Main click function

function onclick(x, y, _button, mod, shift, _caps, opt) {
  if (!inPresetNameTextBounds(x, y) && !inWriteButtonBounds(x, y)) {
    state.set({ editingName: false });
  }

  if (inGridBounds(x, y)) {
    handleCellClick(x, y, {
      recall: shift === 0 && mod === 0 && opt === 0,
      store: shift !== 0 && opt === 0 && mod === 0,
      remove: shift === 0 && mod !== 0 && opt !== 0,
    });

    return;
  }

  if (
    inWriteButtonBounds(x, y) &&
    state.selectedCell !== null &&
    state.editingName
  ) {
    renamePreset(true);
    return;
  }

  if (inPresetNameTextBounds(x, y) && state.selectedCell !== null) {
    state.set({ editingName: !state.editingName });
    return;
  }

  if (
    inConfirmOkButtonBounds(x, y) &&
    (state.confirmStore || state.confirmDelete)
  ) {
    if (state.confirmStore) storePreset();
    if (state.confirmDelete) deletePreset();

    state.set({
      confirmOkDown: true,
      confirmStore: false,
      confirmDelete: false,
    });

    return;
  }

  if (
    inConfirmCancelButtonBounds(x, y) &&
    (state.confirmStore || state.confirmDelete)
  ) {
    state.set({
      confirmCancelDown: true,
      confirmStore: false,
      confirmDelete: false,
    });

    return;
  }
}

// -- Pointer Up

function onpointerup() {
  state.set({
    writeButtonDown: false,
    confirmOkDown: false,
    confirmCancelDown: false,
    confirmStatus:
      state.confirmOkDown || state.confirmCancelDown
        ? null
        : state.confirmStatus,
  });
}

// *-----------------*
// | Keyboard Events |
// *-----------------*

function onkeydown(_, charCode) {
  if (state.presetName === null || !state.editingName) {
    return;
  }

  if (charCode === 127) {
    const presetName = state.presetName.slice(0, -1);
    state.set({ presetName });
  } else if (charCode === 13) {
    renamePreset(false);
  } else {
    const char = String.fromCharCode(charCode);
    state.set({ presetName: state.presetName + char });
  }

  mgraphics.redraw();
}

// *---------*
// | Drawing |
// *---------*

// -- Grid

const drawRow = (y, rowIndex, columns) => {
  for (let i = 0; i < columns; i++) {
    const x = i * CELL_TOTAL_SIZE + CELL_PADDING + GRID_OFFSET_X;
    const cellIndex = i + columns * rowIndex + 1;
    const hovering = cellIndex === state.hoveredCell;
    const selected = cellIndex === state.selectedCell;
    const stored = state.slots && state.slots[cellIndex] !== undefined;

    let color;
    if (selected) {
      color = colors.selectedCell;
    } else if (hovering) {
      color = colors.hoveredCell;
    } else if (stored) {
      color = colors.storedCell;
    } else {
      color = colors.defaultCell;
    }

    mgraphics.set_source_rgba(color);
    mgraphics.rectangle_rounded(x, y, CELL_SIZE, CELL_SIZE, 3, 3);
    mgraphics.fill();
  }
};

const drawRows = () => {
  const [rows, columns] = gridDimensions();

  for (let i = 0; i < rows; i++) {
    const y = i * CELL_TOTAL_SIZE + CELL_PADDING + GRID_OFFSET_Y;
    drawRow(y, i, columns);
  }
};

// -- Buttons

// Abstraction

const drawButton = (x, y, width, height, text, buttonDown, options) => {
  const opts = {
    active: true,
    onColor: colors.writeButtonOn,
    offColor: colors.writeButtonOff,
    ...options,
  };
  const strokeColor = opts.active ? opts.onColor : opts.offColor;

  mgraphics.set_source_rgba(strokeColor);
  mgraphics.rectangle(x, y, width, height);
  mgraphics.set_line_width(1);
  mgraphics.stroke();

  if (buttonDown) {
    mgraphics.set_source_rgba(opts.onColor);
    mgraphics.rectangle(x, y, width, height);
    mgraphics.fill();
  }

  const activeColor = opts.active ? opts.onColor : opts.offColor;
  const textColor = buttonDown ? colors.writeButtonClickText : activeColor;

  drawText(mgraphics, x, y, width, height, text, { color: textColor });
};

// Write button

const drawWriteButton = () => {
  const [x, y, width, height] = writeButtonBounds();
  const { editingName, writeButtonDown } = state;
  drawButton(x, y, width, height, "WRITE NAME", writeButtonDown, {
    active: editingName,
  });
};

// Confirm Button

const drawConfirmButton = (text, bounds, buttonDown) => {
  const [x, y, width, height] = bounds;
  drawButton(x, y, width, height, text, buttonDown, {
    onColor: patcher.color,
  });
};

// -- Preset Text

const drawPresetName = () => {
  const [x, y, width, height] = presetNameTextBounds();
  const { selectedCell, editingName, presetName } = state;

  const selected = selectedCell !== null;
  const editText = editingName ? "> " : "";
  const cursor = editingName ? "_" : "";
  const presetText = editText + presetName + cursor;
  const text = !selected ? "No preset selected" : presetText;

  const face = "Maple Mono";
  const weight = "bold";
  const size = 10;

  if (editingName) {
    mgraphics.select_font_face(face, weight);
    mgraphics.set_font_size(size);

    const [textWidth] = mgraphics.text_measure(text);

    mgraphics.set_source_rgba(colors.presetNameEditingBg);
    mgraphics.rectangle(x, y, textWidth, height);
    mgraphics.fill();
  }

  const editingColor = editingName
    ? colors.presetNameTextEditing
    : colors.presetNameTextDefault;

  const color = !selected ? colors.presetNameTextUnselected : editingColor;

  drawText(mgraphics, x, y, width, height, text, {
    face,
    weight,
    size,
    color,
    alignX: "left",
    alignY: "top",
  });
};

// -- Status Text

const drawStatus = () => {
  const [width, height] = mgraphics.size;
  const {
    status,
    hoveredCell,
    hoveredStatus,
    preferStatus,
    confirmStore,
    confirmDelete,
    confirmOkDown,
    confirmCancelDown,
    confirmStatus,
  } = state;

  const hoveredStatusText = hoveredCell !== null && hoveredStatus;
  const text = preferStatus ? status : hoveredStatusText || status;

  drawText(mgraphics, 8, height - 25, width - 8, 25, confirmStatus || text, {
    face: "Arial",
    weight: "regular",
    color:
      confirmStore || confirmDelete || confirmOkDown || confirmCancelDown
        ? patcher.color
        : colors.statusText,
    alignX: "left",
    alignY: "center",
  });
};

const drawRefreshIcon = () => {
  const [width, height] = mgraphics.size;
  mgraphics.save();
  mgraphics.translate(width - 20, height - 20);

  const scale = 12 / 20;
  mgraphics.scale(scale, scale);

  mgraphics.translate(10, 10);
  mgraphics.rotate(Math.PI * 0.5);
  mgraphics.translate(-10, -10);

  mgraphics.set_source_rgba(
    state.refreshed ? colors.refreshOn : colors.refreshOff,
  );
  mgraphics.set_line_width(1.5);
  mgraphics.set_line_cap("round");

  // Top arc (clockwise arrow)
  mgraphics.arc(10, 10, 6, Math.PI * 0, Math.PI * 1.5);
  mgraphics.stroke();

  // Top arrowhead
  mgraphics.move_to(9, 1);
  mgraphics.line_to(12, 4);
  mgraphics.line_to(9, 7);
  mgraphics.stroke();

  mgraphics.restore();
};

// *------*
// | Main |
// *------*

function paint() {
  clearBackground(mgraphics, colors.bg, 3);
  drawRows();
  drawWriteButton();
  drawPresetName();
  drawStatus();
  if (
    state.confirmStore ||
    state.confirmDelete ||
    state.confirmOkDown ||
    state.confirmCancelDown
  ) {
    drawConfirmButton("OK", confirmOkButtonBounds(), state.confirmOkDown);
    drawConfirmButton(
      "CANCEL",
      confirmCancelButtonBounds(),
      state.confirmCancelDown,
    );
  }
  drawRefreshIcon();
}

function init() {
  outlet(0, "getslotnamelist");
}

init();

// Suppress unused function definitions
void bang;
void anything;
void onfocus;
void onblur;
void onidle;
void onidleout;
void onclick;
void onpointerup;
void onkeydown;
void paint;
