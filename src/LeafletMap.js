import React from "react";
import { useSelector } from "react-redux";
import * as PigeonMap from "pigeon-maps";
import Marker from "pigeon-marker";
import "./Map.scss";

const Map = () => {
  const currentMapMarker = useSelector(state => state.currentMapMarker);
  const markers = useSelector(state => state.markers);
  const markersHistory = useSelector(state => state.markersHistory);
  const savedMarkers = useSelector(state => state.savedMarkers);

  const osm = (x, y, z) => {
    const s = String.fromCharCode(97 + ((x + y + z) % 3));
    return `https://${s}.tile.openstreetmap.fr/hot/${z}/${x}/${y}.png`;
  };

  const currentMarkerLabel = Object.keys(currentMapMarker)[0];
  const markersToDisplay =
    currentMapMarker[currentMarkerLabel].collection === "markers"
      ? markers
      : currentMapMarker[currentMarkerLabel].collection === "markersHistory"
      ? markersHistory
      : currentMapMarker[currentMarkerLabel].collection === "savedMarkers"
      ? savedMarkers
      : null;
  return React.createElement(
    "div",
    {
      className: "Map"
    },
    React.createElement(
      PigeonMap,
      {
        provider: osm,
        limitBounds: "edge",
        center: currentMapMarker[currentMarkerLabel].coordinates[0],
        zoom: currentMapMarker[currentMarkerLabel].coordinates[1]
      },
      Object.keys(markersToDisplay).map(key =>
        React.createElement(Marker, {
          key: key,
          anchor: markersToDisplay[key][0],
          payload: 1
        })
      )
    )
  );
};

module.exports = Map;
