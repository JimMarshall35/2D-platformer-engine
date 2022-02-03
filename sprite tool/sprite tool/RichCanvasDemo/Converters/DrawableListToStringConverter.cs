

namespace RichCanvasDemo.Converters
{
	using RichCanvasDemo.ViewModels;
	using RichCanvasDemo.ViewModels.Base;
	using System;
	using System.Collections.Generic;
	using System.Collections.ObjectModel;
	using System.Globalization;
	using System.Text;
	using System.Windows.Data;
	public class DrawableListToStringConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var items = (ObservableCollection<Drawable>)value;

			string itemsLuaString = "";
			foreach(var item in items)
			{
				if(item is Rectangle)
				{
					itemsLuaString += "[\"" + item.DrawableName + "\"] = { \n";
					itemsLuaString += "\t[\"topleft\"] = { [\"x\"] = " + item.Left.ToString("0.00") + ", [\"y\"] = " + item.Top.ToString("0.00") + "}\n";
					itemsLuaString += "\t[\"dimensions\"] = { [\"x\"] = " + item.Width.ToString("0.00") + ", [\"y\"] = " + item.Height.ToString("0.00") + "}\n";
					itemsLuaString += "}\n";
				}
				
			}
			return itemsLuaString;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
