----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 10:15:50 PM
-- Design Name: 
-- Module Name: mux4_struct - Structural
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity mux4_struct is
    Port ( A : in STD_LOGIC;
           B : in STD_LOGIC;
           A1 : in STD_LOGIC;
           B1 : in STD_LOGIC;
           S : in STD_LOGIC;
           Z : out STD_LOGIC;
           Z1 : out STD_LOGIC);
end mux4_struct;

architecture Structural of mux4_struct is

component mux2_struct
    Port (
        A : in  STD_LOGIC;
        B : in  STD_LOGIC;
        S : in  STD_LOGIC;
        Q : out STD_LOGIC
    );
end component;

begin
MUX0: mux2_struct port map (
    A => A,
    B => B,
    S => S,
    Q => Z
);
MUX1: mux2_struct port map (
    A => A1,
    B => B1,
    S => S,
    Q => Z1
);
end Structural;
