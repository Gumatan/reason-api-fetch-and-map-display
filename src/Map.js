const React = require("react");
const PigeonMap = require("pigeon-maps");

const Map = ({ defaultCenter, children }) => {
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
      defaultCenter,
      zoom: 18,
      children
    })
  );
};

module.exports = Map;
