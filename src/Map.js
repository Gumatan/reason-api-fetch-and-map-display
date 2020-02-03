const React = require("react");
const PigeonMap = require("pigeon-maps");

const Map = ({ message }) => {
  const osm = (x, y, z) => {
    const s = String.fromCharCode(97 + ((x + y + z) % 3));
    return `https://${s}.tile.openstreetmap.fr/hot/${z}/${x}/${y}.png`;
  };

  return React.createElement(
    "div",
    {
      className: "Map"
    },
    React.createElement(PigeonMap, {
      provider: osm,
      limitBounds: "edge",
      center: [50.632787, 3.01809],
      zoom: 18
    })
  );
};

module.exports = Map;
