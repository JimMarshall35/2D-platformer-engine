

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
	using System.Linq;
	public class DrawableListToStringConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var items = (ObservableCollection<Drawable>)value;
			var image = (ImageVisual)items.FirstOrDefault(item => item is ImageVisual);
			if (image == null)
				return "";

			string itemsLuaString = "{\n";
			//itemsLuaString += "\t[\"path\"] = \"" + image.ImageSource + "\";\n";
			itemsLuaString = ConcatWithIndentAndNewline(itemsLuaString, "[\"path\"] = \"" + image.ImageSource + "\";", 1);
			itemsLuaString = ConcatWithIndentAndNewline(itemsLuaString, "[\"named_sprites\"] = {", 1);
			foreach (var item in items)
			{
				if(item is Rectangle)
				{
					itemsLuaString = ConcatWithIndentAndNewline(itemsLuaString, "[\"" + item.DrawableName + "\"] = {", 2);
					itemsLuaString = ConcatWithIndentAndNewline(itemsLuaString, "[\"topleft\"] = { [\"x\"] = " + item.Left.ToString("0") + ", [\"y\"] = " + item.Top.ToString("0") + "};", 3);
					itemsLuaString = ConcatWithIndentAndNewline(itemsLuaString, "[\"dimensions\"] = { [\"x\"] = " + item.Width.ToString("0") + ", [\"y\"] = " + item.Height.ToString("0") + "};", 3);
					itemsLuaString = ConcatWithIndentAndNewline(itemsLuaString, "};", 2);
				}
				
			}
			itemsLuaString = ConcatWithIndentAndNewline(itemsLuaString, "};", 1);
			itemsLuaString += "};\n";
			return itemsLuaString;
		}
		private string ConcatWithIndentAndNewline(string target, string addition, int numtabs)
		{
			string tabs = "";
			for(int i=0; i < numtabs; i++)
			{
				tabs += "\t";
			}
			return target + tabs + addition + "\n";
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
